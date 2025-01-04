#include "Request.hpp"

#include <iostream>

// Constructor

// Default constructor
Request::Request()
{
	std::cout << "\e[2mDefault constructor Request called\e[0m" << std::endl;
	_ready = false;
}

// Copy constructor
Request::Request(const Request &other)
{
	std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Request::~Request()
{
	// std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request &Request::operator=(const Request &other)
{
	std::cout << "\e[2mAssign operator Request called\e[0m" << std::endl;
	if (this != &other)
	{
		_input = other._input;
		_header = other._header;
		_method = other._method;
		_protocol = other._protocol;
		_uri = other._uri;
		_ready = other._ready;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &os, const Request &req)
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

void Request::parseRequest()
{
	std::string line;
	// <Method> <Request-URI> <HTTP-Version>
	std::istringstream _stream(_input);
	std::getline(_stream, line);
	if (line.empty())
		return;
	line = Validate::sanitize(line);
	size_t separator1 = line.find(" ");
	_method = line.substr(0, separator1);
	size_t separator2 = line.find(" ", separator1 + 1);
	_uri =
		Validate::url(line.substr(separator1 + 1, separator2 - separator1 - 1));
	_protocol = line.substr(separator2 + 1, line.length() - separator2);
	if (_protocol != "HTTP/1.1")
		throw HttpError(_protocol + " protocol not supported", 505);
	// field-name: OWS field-value OWS
	while (std::getline(_stream, line))
	{
		line = Validate::sanitize(line);
		if (line.empty())
		{
			_ready = true;
			return;
		}
		auto semi = std::find(line.begin(), line.end(), ':');
		if (semi == line.end())
			throw HttpError("Malformed header field: missing colon separator", 400);
		auto end = std::find_if(line.begin(), semi, [](unsigned char c)
								{ return (c == ' ' || c == '\t'); });
		std::string key = Validate::headerName(std::string(line.begin(), end));
		std::transform(key.begin(), key.end(), key.begin(),
					   [](unsigned char c)
					   { return std::toupper(c); });
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
									  { return (c == ' ' || c == '\t'); });
		end = std::find_if(start, line.end(), [](unsigned char c)
						   { return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end);
		_header.insert(_header.begin(),
					   std::pair<std::string, std::string>(key, value));
	}
}

void Request::append(std::string const &str)
{
	_input.append(str);
}

// Getterss

std::string const &
Request::getMethod() const
{
	return _method;
}

std::string const &Request::getUri() const
{
	return _uri;
}

std::string const &Request::getProtocol() const
{
	return _protocol;
}

const std::map<std::string, std::string> &Request::getHeader() const
{
	return _header;
}

bool Request::isReady() const
{
	return _ready;
}

// Setters
