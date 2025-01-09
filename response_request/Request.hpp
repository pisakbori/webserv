#ifndef REQ_HPP
#define REQ_HPP

#include "HttpError.hpp"
#include "Validate.hpp"
#include "Server.hpp"
#include "CommonIncludes.hpp"

class Request
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::map<std::string, std::string> _header;
	std::string _input;
	std::string _route;
	bool _ready;

	void setRoute(std::string uri, std::string method, const Server &serv);

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
	void parseRequest(const Server &serv);

	// Getters
	std::string const &getProtocol() const;
	std::string const &getUri() const;
	std::string const &getMethod() const;
	std::string const &getRoute() const;
	const std::map<std::string, std::string> &getHeader() const;
	bool isReady() const;
	// Setters
};

std::ostream &operator<<(std::ostream &os, const Request &req);

#endif
