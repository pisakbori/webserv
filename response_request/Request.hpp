#ifndef REQ_HPP
#define REQ_HPP

#include "HttpError.hpp"
#include "Validate.hpp"
#include "Connection.hpp"
#include "CommonIncludes.hpp"

class Connection;
class Request
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::string _body;
	std::map<std::string, std::string> _header;
	std::string _input;

	void validateAllowed(std::string uri, std::string method, const Server &serv);

public:
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
	void parseRequest(Connection *c);

	// Getters
	std::string const &getProtocol() const;
	std::string const &getUri() const;
	std::string const &getMethod() const;
	const std::map<std::string, std::string> &getHeader() const;
	std::string const &getBody() const;
	// Setters
};

std::ostream &operator<<(std::ostream &os, const Request &req);

#endif
