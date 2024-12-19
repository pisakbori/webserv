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
	std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
}

// Destructor
Request::~Request()
{
	std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request &Request::operator=(const Request &other)
{
	(void)other;
	std::cout << "\e[2mAssignation operator Request called\e[0m" << std::endl;
	return (*this);
}

// Member functions

// <Method> <Request-URI> <HTTP-Version>
void Request::parseRequestLine(std::ifstream &stream)
{
	std::string line;
	try
	{
		std::getline(stream, line);

		size_t separator1 = line.find(" ");
		_method = line.substr(0, separator1);
		size_t separator2 = line.find(" ", separator1 + 1);
		_uri = line.substr(separator1 + 1, separator2 - separator1);
		_protocol = line.substr(separator2 + 1, line.length() - separator2);
	}
	catch (std::exception &e)
	{
		throw HttpError("Bad Request: Invalid method", 400);
	}
	// field-name: OWS field-value OWS
	while (std::getline(stream, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		auto semi = std::find(line.begin(), line.end(), ':');
		std::string key = std::string(line.begin(), semi);
		if (!key.length())
			break;
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
									  { return (c == ' ' || c == '\t'); });
		auto end = std::find_if(start, line.end(), [](unsigned char c)
								{ return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end);
		std::cout << "key>" << key << "<" << "value>" << value << "<" << std::endl;
	}
}

// Getters

// Setters
