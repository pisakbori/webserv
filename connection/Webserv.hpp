#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Connection.hpp"
#include "../http_core_module/Parsing.hpp"
#include <set>
#include <sys/types.h>
#include <sys/select.h>
#include <cstring> //for memcpy because FD_COPY wasn't acailable for linux

class Webserv
{
private:
	// key: socketFd
	std::map<int, Connection *> _connections;
	// key: resourceFd, value:socketFd
	std::map<int, int> _resources;
	// key: listeningFd, value: servers index
	std::map<int, std::vector<int>> _listenFdLookup;
	// key: cgiFd, value:socketFd
	std::map<int, int> _cgiFds;
	fd_set _master;
	fd_set _readfds;
	fd_set _writefds;
	fd_set _exceptfds;
	std::vector<Server> _servers;
	int _nReady;

	void onRead(int i);
	void acceptNewConnection(int fd);
	void processRequest(int fd);
	void readFromCGI(int fd);
	void readFromResource(int fd);
	void readFromSocket(int fd);
	void writeToResourceFd(int i);
	int sendOneChunk(Connection *c, int i); //0 on failure
	void onWrite(int fd);
	void removeConnection(int fd);
	void closeResourceFd(int i);
	bool isResource(int i);
	bool isConnection(int fd);
	bool isCGI(int fd);
	int maxFd(void) const;
	void closeConnection(int fd);
	void writeToSocket(Connection *c, int i);
	void writeToCGIStdin(int i);

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
	void closeFd(int i, std::string type = "");
	void printOpenFds() const;
	void closeConnectionResource(int fd);
	// Getters

	// Setters
};

#endif
