#include "Request.hpp"

#include <iostream>

// Constructor

// Default constructor
Request::Request()
{
	throw HttpError("Bad Request: Invalid method", 400);
}

Request::Request(std::ifstream& stream)
{
	parseRequestLine(stream);
}

// Copy constructor
Request::Request(const Request& other)
{
	(void) other;
	// std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
}

// Destructor
Request::~Request()
{
	// std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request& Request::operator=(const Request& other)
{
	(void) other;
	// std::cout << "\e[2mAssignation operator Request called\e[0m" <<
	// std::endl;
	return (*this);
}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
	std::cout << "Method: " << req.getMethod() << std::endl;
	std::cout << "Protocol: " << req.getProtocol() << std::endl;
	std::cout << "URI: " << req.getUri() << std::endl;
	for (auto it = req.getHeader().begin(); it != req.getHeader().end(); ++it)
	{
		std::cout << it->first << ": \"" << it->second << "\"" << std::endl;
	}
	return os;
}

// Member functions

void Request::parseRequestLine(std::ifstream& stream)
{
	std::string line;
	// <Method> <Request-URI> <HTTP-Version>
	std::getline(stream, line);
	line              = Validate::sanitize(line);
	size_t separator1 = line.find(" ");
	_method           = line.substr(0, separator1);
	size_t separator2 = line.find(" ", separator1 + 1);
	_uri =
	    Validate::url(line.substr(separator1 + 1, separator2 - separator1 - 1));
	_protocol = line.substr(separator2 + 1, line.length() - separator2);

	// field-name: OWS field-value OWS
	while (std::getline(stream, line))
	{
		line      = Validate::sanitize(line);
		auto semi = std::find(line.begin(), line.end(), ':');
		if (semi == line.end())
			break;
		auto end        = std::find_if(line.begin(), semi, [](unsigned char c)
		                               { return (c == ' ' || c == '\t'); });
		std::string key = Validate::headerName(std::string(line.begin(), end));
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
		                              { return (c == ' ' || c == '\t'); });
		end        = std::find_if(start, line.end(), [](unsigned char c)
		                          { return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end);

		_header.insert(_header.begin(),
		               std::pair<std::string, std::string>(key, value));
	}
}

// Getters

std::string const& Request::getMethod() const
{
	return _method;
}

std::string const& Request::getUri() const
{
	return _uri;
}

std::string const& Request::getProtocol() const
{
	return _protocol;
}

const std::map<std::string, std::string>& Request::getHeader() const
{
	return _header;
}

// Setters
