#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"

class Connection
{
private:
	Server _server;
	Request _req;
	Response _res;
	// int _fd;

public:
	// Constructor
	Connection();

	// Parameterized constructor

	// Copy constructor
	Connection(const Connection &);

	// Destructor
	~Connection();

	// Overloads
	Connection &operator=(const Connection &);

	// Member functions

	// Getters

	// Setters
};

#endif
