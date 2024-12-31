#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "RunningServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <iostream>

class Connection
{
private:
	RunningServer *_server;
	Response _res;
	int _fd;

public:
	Request _req;
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
	void process();
	void append(std::string const &str);

	// Getters
	int getFd();

	// Setters
	void setFD(int fd);
};

#endif
