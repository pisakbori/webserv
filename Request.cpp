#include "Request.hpp"

#include <iostream>

// Constructor

// Default constructor
Request::Request()
{
	throw HttpError("Bad Request: Invalid method", 400);
}

Request::Request(std::ifstream &stream)
{
	parseRequestLine(stream);
}

// Copy constructor
Request::Request(const Request &other)
{
	(void)other;
	// std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
}

// Destructor
Request::~Request()
{
	// std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request &Request::operator=(const Request &other)
{
	(void)other;
	// std::cout << "\e[2mAssignation operator Request called\e[0m" << std::endl;
	return (*this);
}

// Member functions

void Request::parseRequestLine(std::ifstream &stream)
{
	std::string line;
	// <Method> <Request-URI> <HTTP-Version>
	std::getline(stream, line);
	line = Validate::sanitize(line);
	size_t separator1 = line.find(" ");
	_method = line.substr(0, separator1);
	size_t separator2 = line.find(" ", separator1 + 1);
	_uri = Validate::url(line.substr(separator1 + 1, separator2 - separator1 - 1));
	_protocol = line.substr(separator2 + 1, line.length() - separator2);
		
	// field-name: OWS field-value OWS
	while (std::getline(stream, line))
	{
		line = Validate::sanitize(line);
		// if (!line.empty() && line.back() == '\r')
		// 	line.pop_back();
		auto semi = std::find(line.begin(), line.end(), ':');
		std::string key = Validate::headerName(std::string(line.begin(), semi));
		if (!key.length())
			break;
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
									  { return (c == ' ' || c == '\t'); });
		auto end = std::find_if(start, line.end(), [](unsigned char c)
								{ return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end - 1);
		
		_header.insert(_header.begin(), std::pair<std::string,std::string> (key, value));
	}
}

// Getters

// Setters
