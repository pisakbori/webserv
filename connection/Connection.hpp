#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Cgi.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>

class Request;
class Cgi;

class Connection
{
private:
	const std::vector<Server>& _servers;
	const std::vector<int>& _valid_servers;
	int _responsible_server;
	int _fd;

	Response _res;
	Location _location;
	std::chrono::system_clock::time_point _keepAliveTimeout;
	std::chrono::system_clock::time_point _clientHeaderTimeout;
	int _redirections;
	int getResource(std::filesystem::path path);
	int openResource(std::filesystem::path path);
	int postResource(std::filesystem::path path);
	int deleteResource(std::string path);
	int redirect();
	int setErrorResponse(const std::exception &e);
	int _state;
	bool _close;

public:
	Cgi *_cgi;
	Request *_req;
	size_t _sentBytes;
	size_t _uploadedBytes;
	bool _hasTimeout;
	static constexpr int WAITING_REQ = 0;
	static constexpr int READING_REQ = 1;
	static constexpr int READING_REQ_BODY = 2;
	static constexpr int REQ_READY = 3;
	static constexpr int CGI_READ_REQ_BODY = 4;
	static constexpr int CGI_OUTPUT_READY = 5;
	static constexpr int READING_RESOURCE = 6;
	static constexpr int WRITING_RESOURCE = 7;
	static constexpr int RES_READY = 8;
	static constexpr int RES_SENT = 9;

	// Parameterized constructor
	Connection(const std::vector<Server>&, const std::vector<int>&, int);

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
	void checkTimeout();
	int acceptConnection();
	int getDirectory(std::filesystem::path path);
	void updateKeepAliveTimeout();
	int processCGIOutput();

	// Getters
	const std::vector<Server>& getServ() const;
	const std::vector<int>& getValidServers() const;
	const Server& getResponsibleServer() const;
	int getState() const;
	const Response &getResponse() const;
	const Request *getRequest() const;
	bool hasConnectionClose() const;

	// Setters
	void setState(int s);
	void setResponsibleServer(int i);
};

#endif
