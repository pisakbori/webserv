#include "Connection.hpp"

// Parameterized constructor
Connection::Connection(const std::vector<Server>& servers, const std::vector<int>& valid_idx, int fd) :
	_servers(servers),
	_valid_servers(valid_idx),
	_fd(fd)
{
	_req = new Request();
	_cgi = new Cgi();
	_hasTimeout = false;
	// std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	_state = WAITING_REQ;
	_sentBytes = 0;
	_uploadedBytes = 0;
	_redirections = 0;
	_responsible_server = valid_idx.at(0);
	_close = false;
}

// Copy constructor
Connection::Connection(const Connection &other) :
	_servers(other._servers),
	_valid_servers(other._valid_servers),
	_fd(other._fd)
{
	// std::cout << "\e[2mCopy constructor Connection called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Connection::~Connection()
{
	std::cout << "\e[2mDestructor Connection called\e[0m" << std::endl;
	delete _req;
}

// Overloads
Connection &Connection::operator=(const Connection &other)
{
	// std::cout << "\e[2mAssign operator Connection called\e[0m" << std::endl;
	if (this != &other)
	{
		_fd = other._fd;
		_responsible_server = other._responsible_server;
		_req = other._req;
		_res = other._res;
		_sentBytes = other._sentBytes;
		_uploadedBytes = other._uploadedBytes;
		_hasTimeout = other._hasTimeout;
		_redirections = other._redirections;
		setState(other._state);
	}
	return *this;
}

// Member functions

int Connection::acceptConnection()
{
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	int fd = accept(_fd, (struct sockaddr *)(&cli_addr), &cli_len);

	if (fd < 0)
		throw std::runtime_error("ERROR on accept");
	// TODO::forbidden flag
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("ERROR getting socket flags");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("ERROR setting socket to non-blocking");
	std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
	std::cout << "fd is " << fd << std::endl;
	updateKeepAliveTimeout();
	_clientHeaderTimeout = std::chrono::system_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);

	_cgi->setPipes();

	return fd;
}

// used when connection is established and when data is send or recived
void Connection::updateKeepAliveTimeout()
{
	if (!_hasTimeout)
		_keepAliveTimeout = std::chrono::system_clock::now() + std::chrono::seconds(KEEPALIVE_TIMEOUT);
}

void Connection::reset()
{
	_redirections = 0;
	_hasTimeout = false;
	setState(WAITING_REQ);
	*_req = Request();
	_res = Response();
	_clientHeaderTimeout = std::chrono::system_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
	updateKeepAliveTimeout();
	_sentBytes = 0;
	_uploadedBytes = 0;
	_close = false;
	_cgiResult = "";
	_cgi->setPipes();
};

void Connection::append(std::string const &str)
{
	setState(READING_REQ);
	_req->append(str);
}

void Connection::appendToResponseBody(std::string const &str)
{
	// std::cout << "append to response body " << str << std::endl;
	_res.appendToBody(str);
}

void Connection::appendToCGIResult(std::string const &str)
{
	_cgiResult.append(str);
}

int Connection::getDirectory(std::filesystem::path dirPath)
{
	if (_location.get_autoindex())
	{
		_res = Response(dirPath, _req->getUri());
		setState(Connection::RES_READY);
		return -1;
	}
	else
	{
		if (_location.get_index().empty())
			throw HttpError("Forbidden", 403);
		for (size_t i = 0; i < _location.get_index().size(); i++)
		{
			std::filesystem::path indexFile = _location.get_index()[i];
			std::cout << "indexfile:>" << indexFile << "<" << std::endl;
			std::filesystem::path path = dirPath / indexFile;
			// std::filesystem::path route = uri / indexFile;
			if (std::filesystem::exists(path.string()))
			{
				std::cout << Colors::RED << "Try open resource " << path.string() << std::endl
						  << Colors::RESET;
				return getResource(path);
			}
		}
	}
	throw HttpError("Oh no! " + _req->getUri() + " not found.", 404);
}

int Connection::openResource(std::filesystem::path path)
{
	int resourceFd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	setState(Connection::READING_RESOURCE);
	if (resourceFd == -1)
	{
		throw HttpError("Forbidden, not possible to open resource", 403);
	}
	_res.setContentType(path.extension());
	return resourceFd;
}

int Connection::redirect()
{
	_res.setCode(_location.get_redirect().first);
	_res.appendToHeader("Location", _location.get_redirect().second);
	setState(RES_READY);
	return -1;
}

void Connection::parseCGIOutput()
{
	_res.appendToBody(_cgiResult);
}

int Connection::getResource(std::filesystem::path path)
{
	_redirections++;
	if (_redirections == 10)
		throw HttpError("Too many redirections", 500);
	std::cout << Colors::RED << "GetResource " << path << std::endl
			  << Colors::RESET;
	if (!std::filesystem::exists(path))
		throw HttpError("Oh no! " + _req->getUri() + " not found.", 404);
	else if (std::filesystem::is_directory(path))
		return getDirectory(path);
	else
		return openResource(path);
}

int Connection::postResource(std::filesystem::path path)
{
	// how should this work??? if i send a post request to any endpoint, it will upload to a default directory?
	// only if it doesn't exist? if it exists, act like GET
	if (std::filesystem::exists(path) && !std::filesystem::is_directory(path))
		return openResource(path);
	std::string filename = path.filename().string();
	_res.setCode(201);
	_res.setContentType(".json");
	std::filesystem::path accessLocation = _location.get_uri();
	std::filesystem::path uploadLocation = _location.get_root();
	accessLocation /= filename;
	uploadLocation /= filename;
	if (std::filesystem::exists(uploadLocation))
		throw HttpError(filename + " already exists.", 409);
	_res.appendToHeader("Location", accessLocation.string());
	_res.appendToBody("{\"message\": \"Resource successfully processed.\"}");
	int resourceFd = open(uploadLocation.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0644);
	setState(Connection::WRITING_RESOURCE);
	if (resourceFd == -1)
		throw std::runtime_error("Error uploading file");
	std::cout << "opened " << uploadLocation << std::endl;
	setState(Connection::WRITING_RESOURCE);
	return resourceFd;
}

static bool file_in_use(const std::string &filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
	return !file.is_open();
}

int Connection::deleteResource(std::string uri)
{
	Server server;

	server = getResponsibleServer();
	_location = server.get_location(_req->getUri());
	_location.validate_allowed("DELETE");
	if (_location.get_redirect().first)
		return redirect();
	std::filesystem::path path = _location.get_route(uri);
	if (!std::filesystem::exists(path))
		throw HttpError("Oh no! " + _req->getUri() + " not found.", 404);
	else if (std::filesystem::is_directory(path) && !std::filesystem::is_empty(path))
		throw HttpError("Deleting directories not allowed", 405);
	else
	{
		if (file_in_use(path) && !std::filesystem::is_directory(path))
			throw HttpError("Cannot delete file in use", 409);
		std::filesystem::remove(path);
		_res.setCode(204);
	}
	setState(Connection::RES_READY);
	return -1;
}

int Connection::setErrorResponse(const std::exception &e)
{
	const HttpError *error = dynamic_cast<const HttpError *>(&e);
	if (!error)
	{
		auto err = HttpError(e.what(), 500);
		error = &err;
	}
	err_page_t error_page = getResponsibleServer().get_error_page(error->getCode());
	auto v = error_page.code;
	if (std::find(v.begin(), v.end(), error->getCode()) != v.end())
	{
		std::filesystem::path path;
		_res = Response(*error);
		_res.setBody("");
		if (error_page.overwrite)
			_res.setCode(error_page.overwrite);
		try
		{
			std::filesystem::path errorPagePath = error_page.uri;
			if (errorPagePath.is_absolute())
				_location = getResponsibleServer().get_location(error_page.uri);
			else
				_location = getResponsibleServer().get_location(_req->getUri());
			_location.validate_allowed("GET");
			if (_location.get_redirect().first)
				return redirect();
			if (errorPagePath.is_absolute())
				path = _location.get_route(errorPagePath);
			else
			{
				auto e = HttpError("Found", 302);
				e.setField("LOCATION", error_page.uri);
				throw e;
				// path = _location.get_route(_req->getUri());
				// path = (path.parent_path() / errorPagePath).string();
			}
			return getResource(path);
		}
		catch (HttpError &ex)
		{
			_res = Response(ex);
		}
		catch (const std::exception &e)
		{
			_res = Response(HttpError(e.what(), 500));
		}
	}
	else
		_res = Response(*error);
	setState(Connection::RES_READY);
	return -1;
}

int Connection::process()
{
	try
	{
		_req->parseRequest(this);
		if (_state == REQ_READY)
		{
			// std::cout << *_req << std::endl;
			if (_req->hasConnectionClose())
			{
				_res.appendToHeader("Connection", "close");
				_close = true;
			}

			Server server;
			server = getResponsibleServer();
			_location = server.get_location(_req->getUri());
			_location.validate_allowed(_req->getMethod());
			if (_location.get_redirect().first)
				return redirect();
			std::filesystem::path path = _location.get_route(_req->getUri());

			// replace to match with all cgi extensions
			if (path.extension() == _location.get_cgi_extension())
			{
				_cgi->startCGIprocess(_req, path, _location);
				setState(CGI_READ_REQ_BODY);
				return -1;
			}
			else if (_req->getMethod() == "GET" || _req->getMethod() == "HEAD")
				return getResource(path);
			else if (_req->getMethod() == "POST")
				return postResource(path);
			else if (_req->getMethod() == "DELETE")
				return deleteResource(_req->getUri());
		}
	}
	catch (const std::exception &e)
	{
		return setErrorResponse(e);
	}
	return -1;
}

void Connection::checkTimeout()
{
	try
	{
		auto now = std::chrono::system_clock::now();
		if ((_state == READING_REQ || _state == WAITING_REQ) && now >= _clientHeaderTimeout)
			throw HttpError("Request Header timeout", 408);
		else if (now >= _keepAliveTimeout)
			throw HttpError("Connection expired", 408);
	}
	catch (const HttpError &e)
	{
		_res = Response(e);
		setState(RES_READY);
		_hasTimeout = true;
	}
}

// Getters

const Response &Connection::getResponse() const
{
	return _res;
}

const Request *Connection::getRequest() const
{
	return _req;
}

int Connection::getState() const
{
	return _state;
}

const std::vector<Server> &Connection::getServ() const
{
	return _servers;
}

const std::vector<int>& Connection::getValidServers() const
{
	return _valid_servers;
}

const Server& Connection::getResponsibleServer() const
{
	return _servers.at(_responsible_server);
}

bool Connection::hasConnectionClose() const
{
	return _close;
}

// Setters

int Connection::processCGIOutput()
{
	try
	{
		int status;
		waitpid(_cgi->_cgiPid, &status, 0);
		if (status != 0)
		{
			throw HttpError("CGI did not terminate normally", 500);
		}
		_res.setCGIContent(_cgiResult);
		err_page_t error_page = getResponsibleServer().get_error_page(_res.getCode());
		auto v = error_page.code;
		if (std::find(v.begin(), v.end(), _res.getCode()) != v.end())
			throw HttpError("CGI exited correctly but set error code", _res.getCode());
		_state = RES_READY;
		return -1;
	}
	catch (const HttpError &e)
	{
		return setErrorResponse(e);
	}
	catch (const std::exception &e)
	{
		return setErrorResponse(HttpError(e.what(), 500));
	}
}

void Connection::setState(int s)
{
	if (_state != s)
		std::cout << Colors::YELLOW << "status set from " << _state << " to " << s << std::endl
				  << Colors::RESET;
	_state = s;
	if (s == Connection::RES_READY)
		_res.setContent(_req->getMethod() != "HEAD");
}

void Connection::setResponsibleServer(int i)
{
	_responsible_server = i;
	std::cout << Colors::YELLOW;
	std::cout << "request will be handled by server with index ";
	std::cout << _responsible_server << std::endl << Colors::RESET;
}