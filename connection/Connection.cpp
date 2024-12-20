#include "Connection.hpp"

#include <iostream>

// Constructor
Connection::Connection()
{
	std::cout << "\e[2mDefault constructor Connection called\e[0m" << std::endl;
}

// Parameterized constructor
Connection::Connection(RunningServer *rs)
{
	_server = rs;
	std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	_fd = accept(_server->getListenFd(), (struct sockaddr *)(&cli_addr), &cli_len);
	if (_fd < 0)
		throw std::runtime_error("ERROR on accept");
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("ERROR getting socket flags");
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("ERROR setting socket to non-blocking");
	std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
}

// Copy constructor
Connection::Connection(const Connection &other)
{
	(void)other;
	std::cout << "\e[2mCopy constructor Connection called\e[0m" << std::endl;
}

// Destructor
Connection::~Connection()
{
	std::cout << "\e[2mDestructor Connection called\e[0m" << std::endl;
	close(_fd);
}

// Overloads
Connection &Connection::operator=(const Connection &other)
{
	(void)other;
	std::cout << "\e[2mAssignation operator Connection called\e[0m" << std::endl;
	return (*this);
}

// Member functions

// Getters
int Connection::getFd()
{
	return _fd;
}

// Setters
void Connection::setFD(int fd)
{
	_fd = fd;
}
