#include "Listen.hpp"

#include <iostream>

// Constructor
Listen::Listen() :
	host("0.0.0.0"),
	port(80)
{
	std::cout << "\e[2mDefault constructor Listen called\e[0m" << std::endl;
}

// Copy constructor
Listen::Listen(const Listen &other) :
	host(other.host),
	port(other.port)
{
	std::cout << "\e[2mCopy constructor Listen called\e[0m" << std::endl;
}

// Destructor
Listen::~Listen()
{
	std::cout << "\e[2mDestructor Listen called\e[0m" << std::endl;
}

// Overloads
Listen	&Listen::operator=(const Listen &other)
{
	std::cout << "\e[2mAssignation operator Listen called\e[0m" << std::endl;
	if (this != &other)
	{
		this->host = other.host;
		this->port = other.port;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Listen &listen)
{
	os << "host: " << listen.get_host() << std::endl;
	os << "port: " << listen.get_port() << std::endl;
	return os;
}

// Member functions
void Listen::startListening(void)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw std::runtime_error("Error creating listening socket");
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
	serverAddr.sin_port = htons(port);
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(_fd, (struct sockaddr *)(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::runtime_error("Could not bind socket to port " + std::to_string(serverAddr.sin_port));
	listen(_fd, MAX_QUEUED);
	std::cout << "server: listening on port " << port << std::endl;
};

void Listen::stopListening(void)
{
	close(_fd);
}

// Getters
std::string	Listen::get_host(void) const
{
	return host;
}

unsigned short	Listen::get_port(void) const
{
	return port;
}

int Listen::getFd() const
{
	return _fd;
}

// Setters
void	Listen::setFd(int fd)
{
	_fd = fd;
}

void	Listen::parse_listen(std::string arg)
{
	const size_t	colon_pos = arg.rfind(':');

	if (colon_pos != std::string::npos &&
		(arg.find(']') < colon_pos ||
		arg.find(']') == std::string::npos))
	{
		set_host(arg.substr(0, colon_pos));
		set_port(arg.substr(colon_pos + 1));
	}
	else if (arg.find_first_not_of("0123456789") == std::string::npos)
		set_port(arg);
	else
		set_host(arg);
}

void	Listen::set_host(std::string arg)
{
	char	sa_data[16];

	host = arg;
	if (arg.front() == '[')
	{
		if (inet_pton(AF_INET6, arg.substr(1, arg.size() - 2).c_str(), sa_data) == 1)
			return ; // IPv6
	}
	else if (arg.find_first_not_of("0123456789."))
	{
		if (inet_pton(AF_INET, arg.c_str(), sa_data) == 1)
			return ; // IPv4
	}
	else
		return ; // hostname
}

void	Listen::set_port(std::string arg)
{
	if (arg.find_first_not_of("0123456789") == std::string::npos &&
		std::stoi(arg) > 0 && std::stoi(arg) < 65536)
		port = std::stoi(arg);
}
