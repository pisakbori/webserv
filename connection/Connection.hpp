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

class Request;

class Connection
{
private:
	const Server &_server;
	const Server &_default_server;
	Response _res;
	Request *_req;
	Location _location;
	std::chrono::time_point<std::chrono::high_resolution_clock> _keepAliveTimeout;
	std::chrono::time_point<std::chrono::high_resolution_clock> _clientHeaderTimeout;

	int getResource(std::string path);
	int _state;

public:
	size_t _sentChunks;
	bool _hasTimeout;
	bool _getProcessedByDefault;
	static constexpr int WAITING_REQ = 0;
	static constexpr int READING_REQ_HEADER = 1;
	static constexpr int REQ_READY = 2;
	static constexpr int READING_RESOURCE = 3;
	static constexpr int RES_READY = 4;
	static constexpr int RES_SENT = 5;

	// Parameterized constructor
	Connection(const Server &rs, const Server &def_rs);

	// Copy constructor
	Connection(const Connection &);

	// Destructor
	~Connection();

	// Overloads
	Connection &operator=(const Connection &);

	// Member functions
	int process();
	void append(std::string const &str);
	void appendToResponseBody(std::string const &str);
	void reset();
	void checkTimeout();
	int acceptConnection();
	void handleAutoIndex(std::string path);

	// Getters
	const Server &getServ() const;
	int getState() const;
	const Response &getResponse() const;

	// Setters
	void setState(int s);
};

#endif
