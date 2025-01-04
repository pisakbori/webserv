#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "CommonIncludes.hpp"

class Response
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::string _body;
	std::map<std::string, std::string> _header;

public:
	static const std::map<int, std::string> statuses;
	// Constructor
	Response();
	// Parameterized constructor
	Response(std::string str);

	// Copy constructor
	Response(const Response &);

	// Destructor
	~Response();

	// Overloads
	Response &operator=(const Response &);

	// Member functions

	// Getters
	std::string getBody() const;

	// Setters
};

#endif
