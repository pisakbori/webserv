#include "Connection.hpp"

// Parameterized constructor
Connection::Connection(const Server &rs) : _server(rs)
{
	_req = new Request();
	std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	setState(WAITING_REQ);
}

// Copy constructor
Connection::Connection(const Connection &other) : _server(other._server)
{
	std::cout << "\e[2mCopy constructor Connection called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Connection::~Connection()
{
	std::cout << "\e[2mDestructor Connection called\e[0m" << std::endl;
}

// Overloads
Connection &Connection::operator=(const Connection &other)
{
	std::cout << "\e[2mAssign operator Connection called\e[0m" << std::endl;
	if (this != &other)
	{
		_req = other._req;
		_res = other._res;
		setState(other._state);
	}
	return *this;
}

// Member functions

int Connection::acceptConnection()
{
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	int _fd = accept(_server.getListenFd(), (struct sockaddr *)(&cli_addr), &cli_len);

	if (_fd < 0)
		throw std::runtime_error("ERROR on accept");
	// TODO::forbidden flag
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("ERROR getting socket flags");
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("ERROR setting socket to non-blocking");
	std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
	std::cout << "fd is " << _fd << std::endl;
	_keepAliveTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(KEEPALIVE_TIMEOUT);
	_clientHeaderTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
	return _fd;
}

void Connection::reset()
{
	setState(WAITING_REQ);
	*_req = Request();
	_res = Response();
	_clientHeaderTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
	_sentChunks = 0;
};

void Connection::append(std::string const &str)
{
	setState(READING_REQ_HEADER);
	_req->append(str);
}

void Connection::appendToResponseBody(std::string const &str)
{
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

int Connection::getResource(std::string uri)
{
	_location = _server.get_location(_req->getUri());
	std::string path = _location.get_route(uri);
	std::cout << Colors::RED << "Try open resource " << path << std::endl
			  << Colors::RESET;
	int resourceFd = -1;
	if (path.empty())
		return -1;
	if (!std::filesystem::exists(path))
		throw HttpError("Oh no! " + _req->getUri() + " not found.", 404);
	try
	{
		if (std::filesystem::is_directory(path))
			handleAutoIndex(path);
		else
		{
			resourceFd = open(path.c_str(), O_RDONLY);
			size_t pos = uri.rfind('.');
			if (pos != std::string::npos)
				_res.setContentType(uri.substr(pos + 1));
			else
				_res.setContentType("");
		}
	}
	catch (const std::exception &e)
	{
		throw HttpError(e.what(), 500);
	}
	return resourceFd;
}

int Connection::process()
{
	try
	{
		_req->parseRequest(this);
		if (_state == REQ_READY)
			return getResource(_req->getUri());
	}
	catch (HttpError &e)
	{
		setState(RES_READY);
		auto v = _server.get_error_page().code;
		if (std::find(v.begin(), v.end(), e.getCode()) != v.end())
		{
			_res = Response(e);
			_res.setBody("");
			_res.setCode(_server.get_error_page().overwrite);
			try
			{
				std::cout << Colors::RED << _server.get_error_page().uri << Colors::RESET << std::endl;
				return getResource(_server.get_error_page().uri);
			}
			catch (HttpError &ex)
			{
				_res = Response(ex);

				return -1;
			}

			// TODO:overwrite code
		}

		_res = Response(e);
		std::cout << "error\n";
	}
	return -1;
}

bool Connection::checkTimeout()
{
	auto now = std::chrono::steady_clock::now();
	if ((_state == READING_REQ_HEADER || _state == WAITING_REQ) && now >= _clientHeaderTimeout)
		_res = Response(HttpError("Request Header timeout", 408));
	else if (now >= _keepAliveTimeout)
		_res = Response(HttpError("Connection expired", 408));
	else
		return false;
	_state = TIMEOUT;
	return true;
}

// Getters

const Response &Connection::getResponse() const
{
	return _res;
}

int Connection::getState() const
{
	return _state;
}

const Server &Connection::getServ() const
{
	return _server;
}
// Setters

void Connection::setState(int s)
{
	std::cout << Colors::YELLOW << "status set from " << _state << " to " << s << std::endl
			  << Colors::RESET;
	_state = s;
}