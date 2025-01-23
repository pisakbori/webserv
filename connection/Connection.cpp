#include "Connection.hpp"

// Parameterized constructor
Connection::Connection(const std::vector<Server>& servers, const std::vector<int>& valid_idx, int fd) :
	_servers(servers),
	_valid_servers(valid_idx),
	_fd(fd)
{
	_req = new Request();
	_hasTimeout = false;
	// std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	setState(WAITING_REQ);
	_sentChunks = 0;
	_uploadedBytes = 0;
	_responsible_server = valid_idx.at(0);
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
	// std::cout << "\e[2mDestructor Connection called\e[0m" << std::endl;
}

// Overloads
Connection &Connection::operator=(const Connection &other)
{
	// std::cout << "\e[2mAssign operator Connection called\e[0m" << std::endl;
	if (this != &other)
	{
		_fd = other._fd;
		_req = other._req;
		_res = other._res;
		_sentChunks = other._sentChunks;
		_uploadedBytes = other._uploadedBytes;
		_hasTimeout = other._hasTimeout;
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
	_keepAliveTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(KEEPALIVE_TIMEOUT);
	_clientHeaderTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
	return fd;
}

void Connection::reset()
{
	_hasTimeout = false;
	setState(WAITING_REQ);
	*_req = Request();
	_res = Response();
	_clientHeaderTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
	_sentChunks = 0;
	_uploadedBytes = 0;
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

void Connection::handleAutoIndex(std::string path)
{
	if (!_location.get_autoindex())
		throw std::runtime_error("It's a directory. Should be 404");
	else
	{
		_res = Response(path, _req->getUri());
		setState(Connection::RES_READY);
	}
}

int Connection::openResource(std::string path)
{
	int resourceFd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	setState(Connection::READING_RESOURCE);
	size_t pos = path.rfind('.');
	if (pos != std::string::npos)
		_res.setContentType(path.substr(pos + 1));
	else
		_res.setContentType("");
	return resourceFd;
}

int Connection::redirect()
{
	_res.setCode(_location.get_redirect().first);
	_res.appendToHeader("Location", _location.get_redirect().second);
	_state = RES_READY;
	return -1;
}

int Connection::getResource(std::string uri)
{
	Server server;

	server = _servers.at(_responsible_server);
	_location = server.get_location(_req->getUri());
	if (server.get_location(_req->getUri()).get_redirect().first)
	{
		_res.setCode(server.get_location(_req->getUri()).get_redirect().first);
		_res.appendToHeader("Location", server.get_location(_req->getUri()).get_redirect().second);
		_state = RES_READY;
		return -1;
	}
	std::string path = _location.get_route(uri);
	std::cout << Colors::RED << "Try open resource " << path << std::endl
			  << Colors::RESET;
	if (!std::filesystem::exists(path))
		throw HttpError("Oh no! " + _req->getUri() + " not found.", 404);
	try
	{
		if (std::filesystem::is_directory(path))
			handleAutoIndex(path);
		else
			return openResource(path);
	}
	catch (const std::exception &e)
	{
		throw HttpError(e.what(), 500);
	}
	return -1;
}

int Connection::postResource(std::string uri)
{
	// how should this work??? if i send a post request to any endpoint, it will upload to a default directory?
	// only if it doesn't exist? if it exists, act like GET
	Server server;

	server = _servers.at(_responsible_server);
	_location = server.get_location(_req->getUri());
	if (_location.get_redirect().first)
		return redirect();
	std::string path = _location.get_route(uri);
	if (std::filesystem::exists(path) && !std::filesystem::is_directory(path))
		return openResource(path);

	// set up a basic success response ? ? ? what should be in it ? if ruined, error will show.
	path = _location.get_root();
	try
	{
		std::filesystem::path filePath = uri;
		std::string filename = filePath.filename().string();
		_res.setCode(201);
		_res.setContentType("json");
		std::filesystem::path accessLocation = _location.get_uri();
		std::filesystem::path uploadLocation = path;
		accessLocation /= filename;
		uploadLocation /= filename;
		if (std::filesystem::exists(uploadLocation))
			throw HttpError(filename + " already exists.", 409);
		_res.appendToHeader("Location", accessLocation.string() + "   and upladloc:" + uploadLocation.string());
		_res.appendToBody("{\"message\": \"Resource successfully processed.\"}");
		// TODO:do the uploading etc.
		int resourceFd = open(uploadLocation.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0644);
		std::cout << "opened " << uploadLocation << std::endl;
		setState(Connection::WRITING_RESOURCE);
		if (resourceFd == -1)
		{
			throw std::runtime_error("Error uploading file");
			perror(""); // This will print the error message associated with `errno`
		}
		else
		{
			std::cout << "opened " << uploadLocation << std::endl;
			setState(Connection::WRITING_RESOURCE);
			std::cout << "return  " << resourceFd << std::endl;
		}
		return resourceFd;
		// open a new file
	}
	catch (const HttpError &e)
	{
		throw e;
	}
	catch (const std::exception &e)
	{
		throw HttpError(e.what(), 500);
	}

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
			if (_req->getMethod() == "GET")
				return getResource(_req->getUri());
			else if (_req->getMethod() == "POST")
				return postResource(_req->getUri());
		}
	}
	catch (HttpError &e)
	{
		setState(RES_READY);
		err_page_t error_page = _servers.at(_responsible_server).get_error_page();
		auto v = error_page.code;
		if (std::find(v.begin(), v.end(), e.getCode()) != v.end())
		{
			_res = Response(e);
			_res.setBody("");
			_res.setCode(error_page.overwrite);
			try
			{
				std::cout << Colors::RED << error_page.uri << Colors::RESET << std::endl;
				return getResource(error_page.uri);
			}
			catch (HttpError &ex)
			{
				_res = Response(ex);
				return -1;
			}

			// TODO:overwrite code
		}
		_res = Response(e);
	}
	return -1;
}

void Connection::checkTimeout()
{
	try
	{
		auto now = std::chrono::steady_clock::now();
		if ((_state == READING_REQ || _state == WAITING_REQ) && now >= _clientHeaderTimeout)
			throw HttpError("Request Header timeout", 408);
		else if (now >= _keepAliveTimeout)
			throw HttpError("Connection expired", 408);
	}
	catch (const HttpError &e)
	{
		_res = Response(e);
		_state = RES_READY;
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
// Setters

void Connection::setState(int s)
{
	if (_state != s)
		std::cout << Colors::YELLOW << "status set from " << _state << " to " << s << std::endl
				  << Colors::RESET;
	_state = s;
}

void Connection::setResponsibleServer(int i)
{
	_responsible_server = i;
	std::cout << Colors::YELLOW;
	std::cout << "request will be handled by server with index ";
	std::cout << _responsible_server << std::endl << Colors::RESET;
}