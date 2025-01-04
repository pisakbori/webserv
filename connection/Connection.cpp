#include "Connection.hpp"

// Constructor
Connection::Connection()
{
	std::cout << "\e[2mDefault constructor Connection called\e[0m" << std::endl;
	_server = nullptr;
	_fd = -1;
	_state = WAITING_REQUEST;
}

// Parameterized constructor
Connection::Connection(Server *rs)
{
	_state = WAITING_REQUEST;
	_server = rs;
	std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	_fd = accept(_server->getListenFd(), (struct sockaddr *)(&cli_addr), &cli_len);

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
}

// Copy constructor
Connection::Connection(const Connection &other)
{
	*this = other;
	std::cout << "\e[2mCopy constructor Connection called\e[0m" << std::endl;
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
void Connection::append(std::string const &str)
{
	_state = READING_REQUEST;
	_req.append(str);
}

void Connection::process()
{
	try
	{
		_req.parseRequest();
		if (_req.isReady())
		{
			_res = Response("meow");
			_state = RESPONSE_READY;
		}
	}
	catch (HttpError &e)
	{
		_state = RESPONSE_READY;
		_res = Response(Response::statuses.at(e.code()));
		std::cout << "error\n";
	}
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
void Connection::setFD(int fd)
{
	_fd = fd;
}
