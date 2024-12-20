#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "RunningServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <fcntl.h>

class Connection
{
private:
	RunningServer *_server;
	Request _req;
	Response _res;
	int _fd;

public:
	// Constructor
	Connection();

	// Parameterized constructor
	Connection(RunningServer *rs);

	// Copy constructor
	Connection(const Connection &);

	// Destructor
	~Connection();

	// Overloads
	Connection &operator=(const Connection &);

	// Member functions

	// Getters
	int getFd();

	// Setters
	void setFD(int fd);
};

#endif
