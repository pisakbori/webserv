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
	_nReady = 0;
}

// Parameterized constructor
Webserv::Webserv(std::string configFile)
{
	std::cout << "\e[2mParameterized constructor Webserv called\e[0m" << std::endl;
	FD_ZERO(&_master);
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_ZERO(&_exceptfds);
	_nReady = 0;
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
	std::cout << Colors::GREEN << this->_server << Colors::RESET << std::endl;
}

void Webserv::acceptNewConnection(const Server &server)
{

	std::cout << "new connection" << std::endl;
	auto c = new Connection(server);
	int newfd = c->acceptConnection();
	_connections[newfd] = c;
	FD_SET(newfd, &_master);
}

int Webserv::readFromFd(int fd)
{
	char buf[READ_BUFFER_SIZE];
	ssize_t bytes_read = read(fd, buf, sizeof(buf));
	if (bytes_read > 0)
	{
		std::cout << "read " << bytes_read << " bytes from " << fd << std::endl;
		std::string str(buf, bytes_read);
		if (isResource(fd))
		{
			std::cout << Colors::RED << "Reading from resource fd " << fd << std::endl
					  << Colors::RESET;
			_connections[_resources[fd]]->appendToResponseBody(str);
		}
		else
		{
			_connections[fd]->append(str);
		}
	}
	if (!bytes_read && isResource(fd) && _connections[_resources[fd]]->getState() == Connection::READING_RESOURCE)
	{
		_connections[_resources[fd]]->setState(Connection::RES_READY);
	}
	return bytes_read;
}

bool Webserv::isResource(int fd)
{
	return _resources.find(fd) != _resources.end();
}

void Webserv::closeFd(int fd)
{
	std::cout << "------------------------- Close fd: " << fd << std::endl;
	FD_CLR(fd, &_master);
	close(fd);
}

void Webserv::onRead(int fd)
{
	_nReady--;
	if (fd == _server.getListenFd())
		acceptNewConnection(_server);
	else if (readFromFd(fd) <= 0)
	{
		if (isResource(fd))
		{
			closeFd(fd);
			_resources.erase(fd);
		}
		else if (_connections[fd]->getState() < Connection::READING_RESOURCE)
		{
			delete _connections[fd];
			_connections.erase(fd);
			closeFd(fd);
			// TODO:this is confusing, sorry:(
			// it can be 	READING_RESOURCE or RES_READY and still have read(0); I need to understand.
		}
	}
}

void Webserv::onWrite(int i)
{
	int resourceFd = -1;
	_nReady--;
	auto it = _connections.find(i);
	if (it == _connections.end())
		return;
	auto &c = it->second;
	c->checkTimeout();
	if (c->getState() == Connection::READING_REQ_HEADER)
		resourceFd = c->process();
	if (resourceFd != -1)
	{
		_resources[resourceFd] = i;
		FD_SET(resourceFd, &_master);
		c->setState(Connection::READING_RESOURCE);
		std::cout << Colors::RED << "Add Open resource fd " << resourceFd << std::endl
				  << Colors::RESET;
	}
	if (c->getState() == Connection::RES_READY || c->getState() == Connection::TIMEOUT)
	{
		std::cout << "Sending response to " << i << "\n ";
		std::string response = c->getResponse().toString();
		if (c->_sentChunks * WRITE_BUFFER_SIZE < response.length())
		{
			std::string substring = response.substr(c->_sentChunks * WRITE_BUFFER_SIZE, WRITE_BUFFER_SIZE);
			send(i, substring.c_str(), substring.length(), 0);
			c->_sentChunks++;
		}
		else if (c->getState() == Connection::RES_READY)
			c->reset();
		else
		{
			std::cout << Colors::RED << "Timeout so remove " << i << std::endl
					  << Colors::RESET;
			delete _connections[i];
			_connections.erase(i);
			closeFd(i);
		}
	}
}

int Webserv::maxFd(void) const
{
	int maxSocketFd = _connections.rbegin()->first;
	int maxResourceFd = _resources.empty() ? -1 : _resources.rbegin()->first;
	return std::max(maxSocketFd, maxResourceFd);
}

void Webserv::run()
{
	_server.startListening();
	FD_SET(_server.getListenFd(), &_master);
	int maxfd = _server.getListenFd();

	struct timeval timeout = {0, 0};
	while (1)
	{
		memcpy(&_readfds, &_master, sizeof(_master));
		memcpy(&_writefds, &_master, sizeof(_master));
		if (!_connections.empty())
			maxfd = maxFd();
		if (-1 == (_nReady = select(maxfd + 1, &_readfds, &_writefds, &_exceptfds, &timeout)))
			throw std::runtime_error("select()");
		for (int i = 0; i <= maxfd && _nReady > 0; i++)
		{
			if (FD_ISSET(i, &_readfds))
				onRead(i);
			if (FD_ISSET(i, &_writefds))
				onWrite(i);
			// if (FD_ISSET(i, &_exceptfds))
			// {
			// 	_nReady--;
			// 	std::cout << "ERRRRRRRRRRRRRRRRR " << i << "\n";
			// }
		}
	}
}

void Webserv::stop()
{
	for (auto it = _connections.begin(); it != _connections.end(); ++it)
	{
		close(it->first);
	}
	for (auto it = _resources.begin(); it != _resources.end(); ++it)
	{
		close(it->first);
	}
	std::cout << std::endl
			  << _server.getListenFd() << " fd closed" << std::endl;
	close(_server.getListenFd());
}

// Getters

// Setters
