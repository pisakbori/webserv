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
	std::string _cgiResult;
	std::chrono::time_point<std::chrono::high_resolution_clock> _keepAliveTimeout;
	std::chrono::time_point<std::chrono::high_resolution_clock> _clientHeaderTimeout;

	int getResource(std::string path);
	int _state;

public:
	int _pipefd[2];
	size_t _sentChunks;
	static constexpr int WAITING_REQ = 0;
	static constexpr int READING_REQ_HEADER = 1;
	static constexpr int REQ_READY = 2;
	static constexpr int READING_RESOURCE = 3;
	static constexpr int CGI_OUTPUT_READY = 4;
	static constexpr int RES_READY = 5;
	static constexpr int RES_SENT = 6;
	static constexpr int TIMEOUT = 7;

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
	void appendToCGIResult(std::string const &str);
	void reset();
	bool checkTimeout();
	int acceptConnection();
	void handleAutoIndex(std::string path);
	void executeCGI(std::string path);
	void parseCGIOutput();

	// Getters
	const Server &getServ() const;
	int getState() const;
	const Response &getResponse() const;

	// Setters
	void setState(int s);
};

#endif
