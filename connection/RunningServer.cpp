#include "RunningServer.hpp"

#include <iostream>

// Constructor
RunningServer::RunningServer()
{
	std::cout << "\e[2mDefault constructor RunningServer called\e[0m" << std::endl;
}

// Parameterized constructor
RunningServer::RunningServer(Server &serv)
{
	std::cout << "\e[2mParameterized constructor RunningServer called\e[0m" << std::endl;
	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = inet_addr(serv.get_host().c_str());
	_serverAddr.sin_port = htons(serv.get_port());
	if (bind(_listenFd, (struct sockaddr *)(&_serverAddr), sizeof(_serverAddr)) == -1)
		throw std::runtime_error("Could not bind socket to port " + std::to_string(serv.get_port()));
	listen(_listenFd, MAX_QUEUED);
	std::cout << "server: listening on port " << serv.get_port() << std::endl;
}

// Copy constructor
RunningServer::RunningServer(const RunningServer &other)
{
	*this = other;
	std::cout << "\e[2mCopy constructor RunningServer called\e[0m" << std::endl;
}

// Destructor
RunningServer::~RunningServer()
{
	std::cout << "\e[2mDestructor RunningServer called\e[0m" << std::endl;
	close(_listenFd);
}

// Overloads
RunningServer &RunningServer::operator=(const RunningServer &other)
{
	std::cout << "\e[2mAssign operator RunningServer called\e[0m" << std::endl;
	if (this != &other)
	{
		_listenFd = other._listenFd;
		_serverAddr = other._serverAddr;
	}
	return *this;
}

// Member functions
void RunningServer::run(void) {};

// Getters
struct sockaddr_in RunningServer::getServerAddress()
{
	return _serverAddr;
}

int RunningServer::getListenFd()
{
	return _listenFd;
}

// Setters
