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
	Response _res;
	Request *_req;
	Location _location;
	std::chrono::time_point<std::chrono::high_resolution_clock> _keepAliveTimeout;
	std::chrono::time_point<std::chrono::high_resolution_clock> _clientHeaderTimeout;

	int getResource(std::string path);
	int openResource(std::string path);
	int postResource(std::string path);
	int redirect();
	int setErrorResponse(const HttpError &e);
	int _state;
	bool _close;

public:
	size_t _sentChunks;
	size_t _uploadedBytes;
	bool _hasTimeout;
	static constexpr int WAITING_REQ = 0;
	static constexpr int READING_REQ = 1;
	static constexpr int REQ_READY = 2;
	static constexpr int READING_RESOURCE = 3;
	static constexpr int WRITING_RESOURCE = 4;
	static constexpr int RES_READY = 5;
	static constexpr int RES_SENT = 6;

	// Parameterized constructor
	Connection(const Server &rs);

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
	int getDirectory(std::string path, std::string uri);

	// Getters
	const Server &getServ() const;
	int getState() const;
	const Response &getResponse() const;
	const Request *getRequest() const;
	bool hasConnectionClose() const;

	// Setters
	void setState(int s);
};

#endif
