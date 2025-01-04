#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpError.hpp"
#include "Validate.hpp"
#include "CommonIncludes.hpp"

class Request
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::map<std::string, std::string> _header;
	std::string _input;
	bool _ready;

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
	void parseRequest();

	// Getters
	std::string const &getProtocol() const;
	std::string const &getUri() const;
	std::string const &getMethod() const;
	const std::map<std::string, std::string> &getHeader() const;
	bool isReady() const;
	// Setters
};

std::ostream &operator<<(std::ostream &os, const Request &req);

#endif
