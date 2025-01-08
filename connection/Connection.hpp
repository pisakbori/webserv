#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>

class Connection
{
private:
	Server _server;
	Response _res;
	Request _req;
	int _fd;
	int _state;
	std::chrono::time_point<std::chrono::high_resolution_clock> _keepAliveTimeout;
	std::chrono::time_point<std::chrono::high_resolution_clock> _clientHeaderTimeout;

public:
	static constexpr int WAITING_REQUEST = 0;
	static constexpr int READING_REQUEST_HEADER = 1;
	static constexpr int READING_REQUEST_BODY = 2;
	static constexpr int TIMEOUT = 3;
	static constexpr int RESPONSE_READY = 4;
	static constexpr int IDLE = 5;

	// Constructor
	Connection();

	// Parameterized constructor
	Connection(const Server &rs);

	// Copy constructor
	Connection(const Connection &);

	// Destructor
	~Connection();

	// Overloads
	Connection &operator=(const Connection &);

	// Member functions
	void process();
	void timeout(int duration);
	void append(std::string const &str);
	void reset();
	bool checkTimeout();

	// Getters
	int getFd() const;
	int getState() const;
	const Response &getResponse() const;

	// Setters
	void setFD(int fd);
};

#endif
