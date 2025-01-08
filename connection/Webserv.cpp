#include "Webserv.hpp"

#include <iostream>

// Constructor
Webserv::Webserv()
{
	// TODO:
	std::cout << "\e[2mDefault constructor Webserv called\e[0m" << std::endl;
	FD_ZERO(&_master);
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_ZERO(&_exceptfds);
}

// Parameterized constructor
Webserv::Webserv(std::string configFile)
{
	std::cout << "\e[2mParameterized constructor Webserv called\e[0m" << std::endl;
	this->configure(configFile);
}

// Copy constructor
Webserv::Webserv(const Webserv &other)
{
	(void)other;
	// TODO:
	std::cout << "\e[2mCopy constructor Webserv called\e[0m" << std::endl;
}

// Destructor
Webserv::~Webserv()
{
	// TODO:
	std::cout << "\e[2mDestructor Webserv called\e[0m" << std::endl;
}

// Overloads
Webserv &Webserv::operator=(const Webserv &other)
{
	std::cout << "\e[2mAssign operator Webserv called\e[0m" << std::endl;
	// TODO:
	(void)other;
	return (*this);
}

// Member functions

void Webserv::configure(std::string configFile)
{
	std::ifstream infile(configFile);
	std::string line;
	while (std::getline(infile, line))
	{
		if (line.find("server") == 0)
			_server.populate_server(infile);
	}
}

void Webserv::acceptNewConnection(Server &server)
{

	std::cout << "new connection" << std::endl;
	auto c = new Connection(server);
	int newfd = c->getFd();
	_connections[newfd] = c;
	FD_SET(newfd, &_master);
}

int Webserv::readRequest(int event_fd)
{
	char buf[READ_BUFFER_SIZE];
	size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
	std::cout << "read " << bytes_read << " bytes from " << event_fd << std::endl;
	if (bytes_read)
	{
		std::string str(buf, bytes_read);
		_connections[event_fd]->append(str);
	}
	return bytes_read;
}

void Webserv::removeConnection(int i)
{
	delete _connections[i];
	_connections.erase(i);
	FD_CLR(i, &_master);
	close(i);
}

void Webserv::handleReadyFd(int i)
{
	if (i == _server.getListenFd())
		acceptNewConnection(_server);
	else if (readRequest(i) == 0)
		removeConnection(i);
}

void Webserv::writeResponse(int event_fd)
{
	auto it = _connections.find(event_fd);
	if (it == _connections.end())
		return;
	auto &c = it->second;
	c->checkTimeout();
	if (c->getState() == Connection::READING_REQ_HEADER)
		c->process();
	if (c->getState() == Connection::RES_READY || c->getState() == Connection::TIMEOUT)
	{
		std::cout << "Sending response to " << event_fd << "\n ";
		std::string response = c->getResponse().toString();
		send(event_fd, response.c_str(), response.length(), 0);
		if (c->getState() == Connection::RES_READY)
			c->reset();
		else
			removeConnection(event_fd);
	}
}

void Webserv::run()
{
	int nready;
	_server.startListening();
	FD_SET(_server.getListenFd(), &_master);
	int maxfd = _server.getListenFd();

	struct timeval timeout = {0, 0};
	while (1)
	{
		memcpy(&_readfds, &_master, sizeof(_master));
		memcpy(&_writefds, &_master, sizeof(_master));
		if (!_connections.empty())
			maxfd = _connections.rbegin()->first;
		if (-1 == (nready = select(maxfd + 1, &_readfds, &_writefds, &_exceptfds, &timeout)))
			throw std::runtime_error("select()");
		for (int i = 0; i <= maxfd && nready > 0; i++)
		{
			if (FD_ISSET(i, &_readfds))
			{
				nready--;
				handleReadyFd(i);
			}
			if (FD_ISSET(i, &_writefds))
			{
				nready--;
				writeResponse(i);
			}
			if (FD_ISSET(i, &_exceptfds))
			{
				nready--;
				std::cout << "ERRRRRRRRRRRRRRRRR " << i << "\n";
			}
		}
	}
}

void Webserv::stop()
{
	for (auto it = _connections.begin(); it != _connections.end(); ++it)
	{
		close(it->first);
	}
	std::cout << std::endl
			  << _server.getListenFd() << " fd closed" << std::endl;
	close(_server.getListenFd());
}

// Getters

// Setters
