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
	const Server &_server;
	Response _res;
	Request _req;
	int _fd;
	int _state;
	std::chrono::time_point<std::chrono::high_resolution_clock> _keepAliveTimeout;
	std::chrono::time_point<std::chrono::high_resolution_clock> _clientHeaderTimeout;

	void getResource(std::string path);

public:
	int _resourceFd;

	static constexpr int WAITING_REQ = 0;
	static constexpr int READING_REQ_HEADER = 1;
	static constexpr int READING_REQ_BODY = 2;
	static constexpr int TIMEOUT = 3;
	static constexpr int RES_READY = 4;
	static constexpr int IDLE = 5;
	static constexpr int READING_RESOURCE = 6;

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
	void append(std::string const &str);
	void reset();
	bool checkTimeout();

	// Getters
	int getFd() const;
	int getState() const;
	const Response &getResponse() const;

	// Setters
};

#endif
