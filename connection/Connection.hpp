#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <iostream>

class Connection
{
private:
	Server *_server;
	Response _res;
	Request _req;
	int _fd;
	int _state;

public:
	static constexpr int WAITING_REQUEST = 0;
	static constexpr int READING_REQUEST = 1;
	static constexpr int TIMEOUT = 2;
	static constexpr int RESPONSE_READY = 3;

	// Constructor
	Connection();

	// Parameterized constructor
	Connection(Server *rs);

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
	int getFd() const;
	int getState() const;
	const Response &getResponse() const;

	// Setters
	void setFD(int fd);
};

#endif
