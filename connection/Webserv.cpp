#include "Webserv.hpp"

#include <iostream>

// Constructor
Webserv::Webserv()
{
	// TODO:
	// std::cout << "\e[2mDefault constructor Webserv called\e[0m" << std::endl;
	FD_ZERO(&_master);
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_ZERO(&_exceptfds);
	_nReady = 0;
}

// Parameterized constructor
Webserv::Webserv(std::string configFile)
{
	// std::cout << "\e[2mParameterized constructor Webserv called\e[0m" << std::endl;
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
	// std::cout << "\e[2mCopy constructor Webserv called\e[0m" << std::endl;
}

// Destructor
Webserv::~Webserv()
{
	// TODO:
	// std::cout << "\e[2mDestructor Webserv called\e[0m" << std::endl;
}

// Overloads
Webserv &Webserv::operator=(const Webserv &other)
{
	// std::cout << "\e[2mAssign operator Webserv called\e[0m" << std::endl;
	// TODO:
	(void)other;
	return (*this);
}

// Member functions
void parse_config(std::string file_name, std::vector<Server> &servers)
{
	std::ifstream infile(file_name);
	std::string line;

	while (std::getline(infile, line))
	{
		Server serv;
		if (line.find("server {") != std::string::npos)
			serv.populate_server(infile);
		servers.push_back(serv);
	}
}

void Webserv::configure(std::string configFile)
{
	std::ifstream infile(configFile);
	std::string line;
	parse_config(configFile, _servers);
	std::cout << Colors::GREEN;
	for (size_t i = 0; i < _servers.size(); ++i)
		std::cout << _servers.at(i) << std::endl;
	std::cout << Colors::RESET << std::endl;
}

void Webserv::acceptNewConnection(int fd)
{
	auto c = new Connection(_servers[_listenFdLookup[fd]]);
	int newfd = c->acceptConnection();
	_connections[newfd] = c;
	FD_SET(newfd, &_master);
	printOpenFds();
}

int Webserv::readFromFd(int fd)
{
	char buf[READ_BUFFER_SIZE];
	ssize_t bytes_read = read(fd, buf, sizeof(buf));
	if (bytes_read > 0)
	{
		std::string str(buf, bytes_read);
		if (isResource(fd))
			_connections[_resources[fd]]->appendToResponseBody(str);
		else
			_connections[fd]->append(str);
	}
	if (bytes_read < 0)
		std::cout << Colors::RED << "Oh nooooo, there was a problem when reading from " << fd << Colors::RESET << std::endl;
	return bytes_read;
}

bool Webserv::isResource(int fd)
{
	return _resources.find(fd) != _resources.end();
}

bool Webserv::isConnection(int fd)
{
	return _connections.find(fd) != _connections.end();
}

void Webserv::closeFd(int fd)
{
	std::cout << "------------------------- Close fd: " << fd << std::endl;
	close(fd);
	FD_CLR(fd, &_master);
	printOpenFds();
}

// should i do this inside Connection..?
void Webserv::closeConnectionResource(int fd)
{
	auto it = std::find_if(_resources.begin(), _resources.end(),
						   [fd](const std::pair<int, int> &element)
						   {
							   return element.second == fd;
						   });
	if (it != _resources.end())
	{
		closeFd(it->first);
		_resources.erase(it->first);
	}
}

void Webserv::closeConnection(int fd)
{
	closeConnectionResource(fd);
	delete _connections[fd];
	_connections.erase(fd);
	closeFd(fd);
}

void Webserv::onRead(int fd)
{
	_nReady--;
	if (_listenFdLookup.find(fd) != _listenFdLookup.end())
	{
		acceptNewConnection(fd);
		return;
	}
	if (isResource(fd))
	{
		int bytesRead = readFromFd(fd);
		// std::cout << "read " << bytesRead << " bytes from resource " << fd << std::endl;
		if (bytesRead == 0)
		{
			if (_connections[_resources[fd]]->getState() == Connection::READING_RESOURCE)
				_connections[_resources[fd]]->setState(Connection::RES_READY);
			closeFd(fd);
			_resources.erase(fd);
		}
	}
	else if (isConnection(fd))
	{
		char buf[READ_BUFFER_SIZE];
		ssize_t bytesRead = recv(fd, buf, sizeof(buf), MSG_PEEK);
		if (bytesRead == 0)
		{
			if (_connections[fd]->getState() >= Connection::REQ_READY && _connections[fd]->getState() != Connection::RES_SENT)
			{
				std::cerr << "Client closed the connection, but request processed so we send response" << std::endl;
				return;
			}
			closeConnection(fd);
		}
		else if (bytesRead > 0)
			readFromFd(fd);
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
		printOpenFds();
		c->setState(Connection::READING_RESOURCE);
	}
	if (c->getState() == Connection::RES_READY)
	{
		// std::cout << "Sending response to " << i << "\n ";
		std::string response = c->getResponse().toString();
		if (c->_sentChunks * WRITE_BUFFER_SIZE < response.length())
		{
			std::string substring = response.substr(c->_sentChunks * WRITE_BUFFER_SIZE, WRITE_BUFFER_SIZE);
			int bytesSent = send(i, substring.c_str(), substring.length(), 0);
			c->_sentChunks++;
			if (bytesSent == -1)
			{
				std::cout << Colors::RED << "Client not interested anymore" << Colors::RESET << std::endl;
				closeConnection(i);
				return;
			}
			else
				std::cout << "\e[2mSent " << bytesSent << " bytes to  " << i << "\e[0m" << std::endl;
		}
		else
			c->setState(Connection::RES_SENT);
		if (c->getState() == Connection::RES_SENT)
		{
			if (c->_hasTimeout)
			{
				std::cout << Colors::RED << "Timeout so remove " << i << std::endl
						  << Colors::RESET;
				closeConnection(i);
			}
			else
			{
				c->reset();
				FD_SET(i, &_master);
				printOpenFds();
			}
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
	int maxfd = -1;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		_servers[i].startListening();
		FD_SET(_servers[i].getListenFd(), &_master);
		printOpenFds();
		int max_i = _servers[i].getListenFd();
		if (max_i > maxfd)
			maxfd = max_i;
		_listenFdLookup[_servers[i].getListenFd()] = i;
	}

	struct timeval timeout = {0, 0};
	while (1)
	{
		memcpy(&_readfds, &_master, sizeof(_master));
		memcpy(&_writefds, &_master, sizeof(_master));
		memcpy(&_exceptfds, &_master, sizeof(_master));
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
			if (FD_ISSET(i, &_exceptfds))
			{
				_nReady--;
				// std::cout << "ERRRRRRRRRRRRRRRRR " << i << "\n";
				// int err = 0;
				// socklen_t len = sizeof(err);
				// if (isConnection(i) && getsockopt(i, SOL_SOCKET, SO_ERROR, &err, &len) == 0)
				// {
				// 	if (err != 0)
				// 	{
				// 		std::cerr << "Socket error on FD " << i << ": " << strerror(err) << std::endl;
				// 	}
				// }
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
	for (auto it = _resources.begin(); it != _resources.end(); ++it)
	{
		close(it->first);
	}
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << std::endl
				  << _servers[i].getListenFd() << " fd closed" << std::endl;
		close(_servers[i].getListenFd());
	}
}

void Webserv::printOpenFds() const
{
	std::cout << "Open fds: " << Colors::RED;
	for (int fd = 0; fd < 100; fd++)
	{
		if (FD_ISSET(fd, &_master))
			std::cout << fd << " ";
	}
	std::cout << std::endl
			  << Colors::RESET;
}

// Getters

// Setters
