#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "HttpError.hpp"
#include "Validate.hpp"

class Request
{
private:
	std::string _method;
	std::string _uri;
	std::string _protocol;
	std::map<std::string, std::string> _header;
	Request();

public:
	// Constructor
	Request(std::ifstream &stream);

	// Parameterized constructor

	// Copy constructor
	Request(const Request &);

	// Destructor
	~Request();

	// Overloads
	Request &operator=(const Request &);

	// Member functions

	void parseRequestLine(std::ifstream &stream);
	// Getters

	// Setters
};

#endif
