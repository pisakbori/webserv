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

void Webserv::acceptNewConnection(const Server &server)
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
	size_t bytes_read = read(event_fd, buf, sizeof(buf));
	if (bytes_read)
	{
		std::cout << "read " << bytes_read << " bytes from " << event_fd << std::endl;
		std::string str(buf, bytes_read);
		if (event_fd == _connections[event_fd]->getFd())
		{
			_connections[event_fd]->append(str);
		}
		else
		{
			std::cout << Colors::RED << "Reading from resource fd " << event_fd << std::endl
					  << Colors::RESET;
			_connections[event_fd]->appendToResponseBody(str);
		}
	}
	if (!bytes_read && _connections[event_fd]->getState() == Connection::READING_RESOURCE && _connections[event_fd]->_resourceFd == event_fd)
	{
		_connections[event_fd]->setState(Connection::RES_READY);
	}
	return bytes_read;
}

void Webserv::removeConnection(int i)
{

	std::cout << Colors::RED << "Delete connection at fd " << i << std::endl
			  << Colors::RESET;
	delete _connections[i];
	_connections.erase(i);
	FD_CLR(i, &_master);
	close(i);
}

void Webserv::closeResourceFd(int i)
{
	std::cout << Colors::RED << "Close resource fd " << i << std::endl
			  << Colors::RESET;
	_connections.erase(i);
	FD_CLR(i, &_master);
	close(i);
}

void Webserv::handleReadyFd(int i)
{
	if (i == _server.getListenFd())
		acceptNewConnection(_server);
	else
	{
		int readBytes = readRequest(i);
		if (readBytes)
			return;
		if (_connections[i]->getFd() == i && _connections[i]->getState() < Connection::READING_RESOURCE)
		{
			std::cout << Colors::RED << "0 bytes read, connection interrupted, so remove " << i << std::endl
					  << Colors::RESET;
			removeConnection(i);
		}
		else if (_connections[i]->_resourceFd == i)
		{
			std::cout << Colors::RED << "0 bytes read, close resource fd " << i << std::endl
					  << Colors::RESET;
			closeResourceFd(i);
		}
	}
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
	if (c->getState() == Connection::REQ_READY && c->_resourceFd != -1)
	{
		_connections[c->_resourceFd] = c;
		FD_SET(c->_resourceFd, &_master);
		c->setState(Connection::READING_RESOURCE);
		std::cout << Colors::RED << "Add Open resource fd " << c->_resourceFd << std::endl
				  << Colors::RESET;
	}
	if (c->getState() == Connection::RES_READY || c->getState() == Connection::TIMEOUT)
	{
		std::cout << "Sending response to " << c->getFd() << "\n ";
		std::string response = c->getResponse().toString();
		send(c->getFd(), response.c_str(), response.length(), 0);
		if (c->getState() == Connection::RES_READY)
			c->reset();
		else
		{
			std::cout << Colors::RED << "Timeout so remove " << event_fd << std::endl
					  << Colors::RESET;
			removeConnection(event_fd);
		}
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
