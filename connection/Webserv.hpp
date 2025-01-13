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
	fd_set _master;
	fd_set _readfds;
	fd_set _writefds;
	fd_set _exceptfds;
	// TODO:make it list
	Server _server;
	int _nReady;

	void onRead(int i);
	void acceptNewConnection(const Server &server);
	void processRequest(int event_fd);
	int readFromFd(int event_fd);
	void onWrite(int event_fd);
	void removeConnection(int event_fd);
	void closeResourceFd(int i);
	bool isResource(int i);
	int maxFd(void) const;

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
	// Getters

	// Setters
};

#endif
