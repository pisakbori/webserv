#ifndef RUNNINGSERVER_HPP
#define RUNNINGSERVER_HPP

#include <string>
#include "Server.hpp"
#include "CommonIncludes.hpp"

class RunningServer
{
private:
	struct sockaddr_in _serverAddr;
	int _listenFd;

public:
	// Constructor
	RunningServer();

	// Parameterized constructor
	RunningServer(Server &serv);

	// Copy constructor
	RunningServer(const RunningServer &);

	// Destructor
	~RunningServer();

	// Overloads
	RunningServer &operator=(const RunningServer &);

	// Member functions
	void run(void);

	// Getters
	struct sockaddr_in getServerAddress();
	int getListenFd();

	// Setters
};

#endif
