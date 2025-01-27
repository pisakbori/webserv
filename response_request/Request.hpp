#ifndef REQ_HPP
#define REQ_HPP

#include "HttpMessage.hpp"
#include "Connection.hpp"

class Connection;
class Request : public HttpMessage
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::string _body;
	std::string _input;

public:
	size_t _bodySize;
	// Constructor
	Request();

	// Parameterized constructor

	// Copy constructor
	Request(const Request &);

	// Destructor
	~Request();

	// Overloads
	Request &operator=(const Request &);

	// Member functions
	void append(std::string const &str);
	void matchHost(Connection *c);
	void parseRequestLine(std::string &line);
	void parseContentLength(Connection *c, std::istringstream &stream);
	void parseRequest(Connection *c);

	// Getters
	std::string const &getProtocol() const;
	std::string const &getUri() const;
	std::string const &getMethod() const;
	const std::map<std::string, std::string> &getHeader() const;
	bool hasConnectionClose() const;
	std::string const &getBody() const;
	// Setters
};

std::ostream &operator<<(std::ostream &os, const Request &req);

#endif
