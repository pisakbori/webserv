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
	// std::cout << "\e[2mDestructor Webserv called\e[0m" << std::endl;
    for (auto& pair : _connections) {
        delete pair.second; 
    }
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
void Webserv::configure(std::string configFile)
{
	parse_config(configFile, _servers);
	std::cout << Colors::GREEN;
	for (size_t i = 0; i < _servers.size(); ++i)
		std::cout << _servers.at(i) << std::endl;
	std::cout << Colors::RESET << std::endl;
}

void Webserv::acceptNewConnection(int fd)
{
	auto c = new Connection(_servers, _listenFdLookup[fd], fd);
	int newfd = c->acceptConnection();
	_connections[newfd] = c;
	FD_SET(newfd, &_master);

	_cgiFds[c->_cgi->_cgi2parent[0]] = newfd;
	_cgiFds[c->_cgi->_parent2cgi[1]] = newfd;
	FD_SET(c->_cgi->_cgi2parent[0], &_master);
	FD_SET(c->_cgi->_parent2cgi[1], &_master);
	printOpenFds();
}

bool Webserv::isResource(int fd)
{
	return _resources.find(fd) != _resources.end();
}

bool Webserv::isConnection(int fd)
{
	return _connections.find(fd) != _connections.end();
}

bool Webserv::isCGI(int fd)
{
	return _cgiFds.find(fd) != _cgiFds.end();
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

void Webserv::closeFd(int fd, std::string type)
{
	if (fd == -1)
		return;
	std::cout << "------------------------- Close fd: " << fd << " " << type << std::endl;
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
		closeFd(it->first, "connecion resource");
		_resources.erase(it->first);
	}
}

void Webserv::closeConnectionCgi(int fd)
{
	std::cout << "heree killing connection " << fd << std::endl;
	_connections[fd]->_cgi->killCgi();
	_cgiFds.erase(_connections[fd]->_cgi->_cgi2parent[0]);
	_cgiFds.erase(_connections[fd]->_cgi->_cgi2parent[1]);
	_cgiFds.erase(_connections[fd]->_cgi->_parent2cgi[0]);
	_cgiFds.erase(_connections[fd]->_cgi->_parent2cgi[1]);
	closeFd(_connections[fd]->_cgi->_cgi2parent[0]);
	closeFd(_connections[fd]->_cgi->_cgi2parent[1]);
	closeFd(_connections[fd]->_cgi->_parent2cgi[0]);
	closeFd(_connections[fd]->_cgi->_parent2cgi[1]);
	_connections[fd]->_cgi->_cgi2parent[0] = -1;
	_connections[fd]->_cgi->_cgi2parent[1] = -1;
	_connections[fd]->_cgi->_parent2cgi[0] = -1;
	_connections[fd]->_cgi->_parent2cgi[1] = -1;
}

void Webserv::closeConnection(int fd)
{
	std::cout << "close connection\n";
	closeConnectionResource(fd);
	closeConnectionCgi(fd);
	delete _connections[fd];
	_connections.erase(fd);
	closeFd(fd, "socket");
}

void Webserv::readFromResource(int fd)
{
	char buf[READ_BUFFER_SIZE];
	ssize_t bytesRead = read(fd, buf, sizeof(buf));
	if (bytesRead > 0)
	{
		// std::cout << "\e[2mRead " << bytesRead << " bytes from resource " << fd << "\e[0m" << std::endl;
		std::string str(buf, bytesRead);
		_connections[_resources[fd]]->appendToResponseBody(str);
	}
	else if (bytesRead < 0)
	{
		std::cout << Colors::RED << "Oh nooooo, there was a problem when reading from " << fd << Colors::RESET << std::endl;
		// TODO:Bori disconnect?
	}
	else if (bytesRead == 0)
	{
		std::cout << "\e[2mFinished reading resource " << fd << "\e[0m" << std::endl;
		_connections[_resources[fd]]->setState(Connection::RES_READY);
		closeFd(fd, "read resource");
		_resources.erase(fd);
	}
}

void Webserv::readFromCGI(int fd)
{
	char buf[READ_BUFFER_SIZE];
	ssize_t bytesRead = read(fd, buf, sizeof(buf));
	if (bytesRead > 0)
	{
		std::cout << "\e[2mRead " << bytesRead << " bytes from CGI " << fd << "\e[0m" << std::endl;
		std::string str(buf, bytesRead);
		_connections[_cgiFds[fd]]->appendToCGIResult(str);
	}
	else if (bytesRead < 0)
	{
		std::cout << Colors::RED << "Oh nooooo, there was a problem when reading from CGI" << fd << Colors::RESET << std::endl;
		// TODO:Bori disconnect?
	}
	else if (bytesRead == 0)
	{
		std::cout << "read " << bytesRead << " bytes from CGI " << fd << std::endl;
		_connections[_cgiFds[fd]]->setState(Connection::CGI_OUTPUT_READY);
		int resourceFd = _connections[_cgiFds[fd]]->processCGIOutput();
		if (resourceFd != -1)
		{
			std::cout << "set resource " << resourceFd << std::endl;
			_resources[resourceFd] = _cgiFds[fd];
			FD_SET(resourceFd, &_master);
			printOpenFds();
		}
		closeConnectionCgi(_cgiFds[fd]);
	}
}

void Webserv::readFromSocket(int fd)
{
	char buf[READ_BUFFER_SIZE];
	ssize_t bytesRead = read(fd, buf, sizeof(buf));
	if (bytesRead > 0)
	{
		std::string str(buf, bytesRead);
		std::cout << "read " << bytesRead << " from socket" << std::endl;
		_connections[fd]->updateKeepAliveTimeout();
		_connections[fd]->append(str);
		if (_connections[fd]->getState() == Connection::READING_REQ || _connections[fd]->getState() == Connection::READING_REQ_BODY)
		{
			int resourceFd = _connections[fd]->process();
			if (resourceFd != -1)
			{
				std::cout << "set resource " << resourceFd << std::endl;
				_resources[resourceFd] = fd;
				FD_SET(resourceFd, &_master);
				printOpenFds();
			}
			else if (_connections[fd]->getState() == Connection::CGI_READ_REQ_BODY)
			{
				closeFd(_connections[fd]->_cgi->_cgi2parent[1], "_cgi2parent write: useless");
				closeFd(_connections[fd]->_cgi->_parent2cgi[0], "_parent2cgi read: useless");
				_cgiFds.erase(_connections[fd]->_cgi->_cgi2parent[1]);
				_cgiFds.erase(_connections[fd]->_cgi->_parent2cgi[0]);
				_connections[fd]->_cgi->_cgi2parent[1] = -1;
				_connections[fd]->_cgi->_parent2cgi[0] = -1;
			}
		}
	}
	else if (bytesRead < 0)
	{
		std::cout << Colors::RED << "Oh nooooo, there was a problem when reading from " << fd << Colors::RESET << std::endl;
		closeConnection(fd);
	}
	else if (bytesRead == 0)
	{
		// std::cout << Colors::RED << "bytesRead 0 " << fd << Colors::RESET << std::endl;
		if (_connections[fd]->getState() >= Connection::REQ_READY && _connections[fd]->getState() != Connection::RES_SENT)
		{
			// std::cerr << "Client closed the connection, but request processed so we send response" << std::endl;
			return;
		}
		std::cout << "==========================================Close connection" << std::endl;
		closeConnection(fd);
	}
}

void Webserv::writeToResourceFd(int i)
{
	auto c = _connections[_resources[i]];
	size_t size = c->getRequest()->_bodySize;
	if (size > 0 && c->_uploadedBytes < size)
	{
		std::string substring = c->getRequest()->getBody().substr(c->_uploadedBytes, WRITE_BUFFER_SIZE);
		int uploadedBytes = write(i, substring.c_str(), substring.size());
		if (uploadedBytes == -1)
		{
			std::cout << "TODO:BORI here i should set them an 500" << std::endl;
			closeConnection(_resources[i]);
			return;
		}
		else if (uploadedBytes > 0)
		{
			// std::cout << "\e[2mUploaded " << uploadedBytes << " bytes to  " << i << "\e[0m" << std::endl;
			c->_uploadedBytes += uploadedBytes;
		}
		else
		{
			std::cout << "TODO:Bori    no space left?? shuld i respond internal server error?" << std::endl;
			closeConnection(_resources[i]);
			return;
		}
	}
	else
	{
		c->setState(Connection::RES_READY);
		closeFd(i, "upload resource");
		_resources.erase(i);
	}
}

void Webserv::writeToCGIStdin(int i)
{
	auto c = _connections[_cgiFds[i]];
	if (c->getState() != Connection::CGI_READ_REQ_BODY)
		return;

	size_t size = c->getRequest()->_bodySize;
	if (size > 0 && c->_uploadedBytes < size)
	{
		std::string substring = c->getRequest()->getBody().substr(c->_uploadedBytes, WRITE_BUFFER_SIZE);
		int uploadedBytes = write(i, substring.c_str(), substring.size());
		if (uploadedBytes == -1)
		{
			// TODO:Bori - should i even close connection or just throw 500?
			closeConnection(_cgiFds[i]);
			return;
		}
		else if (uploadedBytes > 0)
		{
			// std::cout << "\e[2mUploaded " << uploadedBytes << " bytes to  " << i << "\e[0m" << std::endl;
			c->_uploadedBytes += uploadedBytes;
		}
		else
		{
			// TODO:Bori    no space left?? shuld i respond internal server error?
			closeConnection(_cgiFds[i]);
			return;
		}
	}
	else
	{
		std::cout << "finished writing request\n";
		// c->setState(Connection::CGI_WRITE_OUTPUT);
		closeFd(i, "finished writing request body to CGI");
		_cgiFds.erase(i);
		c->_cgi->_parent2cgi[1] = -1;
	}
}

int Webserv::sendOneChunk(Connection *c, int i)
{
	std::string substring = c->getResponse().getContent(c->_sentBytes, WRITE_BUFFER_SIZE);
	int bytesSent = send(i, substring.c_str(), substring.length(), 0);
	if (bytesSent == -1)
	{
		std::cout << Colors::RED << "Error sending response." << Colors::RESET << std::endl;
		closeConnection(i);
	}
	else if (bytesSent > 0)
	{
		c->updateKeepAliveTimeout();
		c->_sentBytes += bytesSent;
		// std::cout << "\e[2mSent " << bytesSent << " bytes to  " << i << "\e[0m" << std::endl;
		return 1;
	}
	else if (bytesSent == 0)
	{

		std::cout << Colors::RED << "0 sent.....????" << Colors::RESET << std::endl;
		closeConnection(i);
	}
	return 0;
}

void Webserv::writeToSocket(Connection *c, int i)
{
	// std::cout << "Sending response to " << i << "\n ";
	try
	{
		if (c->_sentBytes < c->getResponse().getSize())
		{
			if (!sendOneChunk(c, i))
			return;
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
			else if (c->hasConnectionClose())
			{
				std::cout << Colors::RED << "Request had Connection Close so remove " << i << std::endl
						  << Colors::RESET;
				closeConnection(i);
			}
			else
			{
				_cgiFds.erase(_connections[i]->_cgi->_cgi2parent[0]);
				_cgiFds.erase(_connections[i]->_cgi->_parent2cgi[1]);
				closeConnectionCgi(i);
				c->reset();
				_cgiFds[c->_cgi->_cgi2parent[0]] = i;
				FD_SET(c->_cgi->_cgi2parent[0], &_master);
				_cgiFds[c->_cgi->_parent2cgi[1]] = i;
				FD_SET(c->_cgi->_parent2cgi[1], &_master);
				printOpenFds();
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}

void Webserv::onWrite(int i)
{
	_nReady--;
	if (isResource(i) && _connections[_resources[i]]->getState() == Connection::WRITING_RESOURCE)
		writeToResourceFd(i);
	else if (isConnection(i))
	{
		auto c = _connections[i];
		c->checkTimeout();
		if (c->getState() == Connection::RES_READY)
			writeToSocket(c, i);
	}
	else if (isCGI(i) && _connections[_cgiFds[i]]->_cgi->_parent2cgi[1] == i)
	{
		writeToCGIStdin(i);
	}
}

void Webserv::onRead(int fd)
{
	_nReady--;
	if (_listenFdLookup.find(fd) != _listenFdLookup.end())
		acceptNewConnection(fd);
	else if (isResource(fd) && _connections[_resources[fd]]->getState() == Connection::READING_RESOURCE)
		readFromResource(fd);
	else if (isConnection(fd))
		readFromSocket(fd);
	else if (isCGI(fd))
		readFromCGI(fd);
}

int Webserv::maxFd(void) const
{
	int maxSocketFd = _connections.rbegin()->first;
	int maxResourceFd = _resources.empty() ? -1 : _resources.rbegin()->first;
	int maxCGIFd = _cgiFds.empty() ? -1 : _cgiFds.rbegin()->first;
	int max = std::max(maxSocketFd, maxResourceFd);
	return std::max(max, maxCGIFd);
}

void Webserv::run()
{
	std::map<Listen, int> listenFdMap;
	int maxfd = -1;

// TODO:Marian there is somehow a leak related to listenFdMap. please take a look
	for (size_t i = 0; i < _servers.size(); i++)
	{
		Listen& listen = _servers[i].get_listen();
		if (listenFdMap.find(listen) == listenFdMap.end())
		{
			try
			{
				listen.startListening();
				listenFdMap[listen] = listen.getFd();
				FD_SET(listen.getFd(), &_master);
				maxfd = std::max(maxfd, listen.getFd());
			}
			catch (const std::runtime_error & e)
			{
				std::cerr << e.what() << std::endl;
				// close current fd
				listen.stopListening();
				// close all previous fds
				for (const auto & [listen, fd] : listenFdMap)
					close(fd);
				FD_ZERO(&_master);
				std::exit(1);
			}
		}
		else
			listen.setFd(listenFdMap[listen]);
		_listenFdLookup[listen.getFd()].push_back(i);
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
			throw std::runtime_error(strerror(errno));
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
		closeFd(it->first, "socket");
	}
	for (auto it = _resources.begin(); it != _resources.end(); ++it)
	{
		closeFd(it->first, "resource");
	}
	for (auto it = _listenFdLookup.begin(); it != _listenFdLookup.end(); ++it)
	{
		closeFd(it->first, "listening");
	}
	for (auto it = _cgiFds.begin(); it != _cgiFds.end(); ++it)
	{
		closeFd(it->first);
	}
}

// Getters

// Setters
