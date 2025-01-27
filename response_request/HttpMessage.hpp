#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include "CommonIncludes.hpp"
#include "Validate.hpp"
#include "HttpError.hpp"

class HttpMessage
{
protected:
	std::map<std::string, std::string> _header;

public:
	// Constructor
	HttpMessage();

	// Parameterized constructor

	// Copy constructor
	HttpMessage(const HttpMessage &);

	// Destructor
	~HttpMessage();

	// Overloads
	HttpMessage &operator=(const HttpMessage &);

	// Member functions
	void parseFieldLine(std::string &line, bool *headerRead, int errorCode);

	// Getters

	// Setters
};

#endif
