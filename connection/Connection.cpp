#include "Connection.hpp"

// Constructor
Connection::Connection()
{
	std::cout << "\e[2mDefault constructor Connection called\e[0m" << std::endl;
	_fd = -1;
	_state = WAITING_REQ;
}

// Parameterized constructor
Connection::Connection(const Server &rs)
{

	std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	_state = WAITING_REQ;
	_server = rs;
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	_fd = accept(_server.getListenFd(), (struct sockaddr *)(&cli_addr), &cli_len);

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
}

// Copy constructor
Connection::Connection(const Connection &other)
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
		_server = other._server;
		_req = other._req;
		_res = other._res;
		_fd = other._fd;
		_state = other._state;
	}
	return *this;
}

// Member functions

void Connection::reset()
{
	_state = WAITING_REQ;
	_req = Request();
	_res = Response();
	_clientHeaderTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(CLIENT_HEADER_TIMEOUT);
};

void Connection::append(std::string const &str)
{
	_state = READING_REQ_HEADER;
	_req.append(str);
}

void Connection::process()
{
	try
	{
		_req.parseRequest(_server);
		if (_req.isReady())
		{
			_res = Response("meow");
			_state = RES_READY;
		}
	}
	catch (HttpError &e)
	{
		_state = RES_READY;
		_res = Response(e);
		std::cout << "error\n";
	}
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
int Connection::getFd() const
{
	return _fd;
}

const Response &Connection::getResponse() const
{
	return _res;
}

int Connection::getState() const
{
	return _state;
}
// Setters
