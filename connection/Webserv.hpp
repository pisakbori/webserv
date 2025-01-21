#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Connection.hpp"
#include <sys/types.h>
#include <sys/select.h>

class Webserv
{
private:
	std::map<int, Connection *> _connections;
	// key: resourceFd, value:socketFd
	std::map<int, int> _resources;
	// key: listeningFd, value:serverIndex
	std::map<int, int> _listenFdLookup;
	fd_set _master;
	fd_set _readfds;
	fd_set _writefds;
	fd_set _exceptfds;
	// TODO:make it list
	std::vector<Server> _servers;
	int _nReady;

	void onRead(int i);
	void acceptNewConnection(int fd);
	void processRequest(int fd);
	int readFromFd(int fd);
	void writeToResourceFd(int i);
	void onWrite(int fd);
	void removeConnection(int fd);
	void closeResourceFd(int i);
	bool isResource(int i);
	bool isConnection(int i);
	int maxFd(void) const;
	void closeConnection(int fd);
	void sendResponseChunk(Connection *c, int i);

public:
	// Constructor
	Webserv();

	// Parameterized constructor
	Webserv(std::string configFile);

	// Copy constructor
	Webserv(const Webserv &);

	// Destructor
	~Webserv();

	// Overloads
	Webserv &operator=(const Webserv &);

	// Member functions
	void configure(std::string configFile);
	void run(void);
	void stop();
	void closeFd(int i);
	void printOpenFds() const;
	void closeConnectionResource(int fd);
	// Getters

	// Setters
};

#endif
