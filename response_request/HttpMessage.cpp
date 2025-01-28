#include "HttpMessage.hpp"

#include <iostream>

// Constructor
HttpMessage::HttpMessage()
{
	std::cout << "\e[2mDefault constructor HttpMessage called\e[0m" << std::endl;
}

// Parameterized constructor
// HttpMessage::HttpMessage()
// {
// 	std::cout << "\e[2mParameterized constructor HttpMessage called\e[0m" << std::endl;
// }

// Copy constructor
HttpMessage::HttpMessage(const HttpMessage &other)
{
	*this = other;
	// std::cout << "\e[2mCopy constructor HttpMessage called\e[0m" << std::endl;
}

// Destructor
HttpMessage::~HttpMessage()
{
	// std::cout << "\e[2mDestructor HttpMessage called\e[0m" << std::endl;
}

// Overloads
HttpMessage &HttpMessage::operator=(const HttpMessage &other)
{
	if (this != &other)
	{
		_header = other._header;
	}
	std::cout << "\e[2mAssign operator HttpMessage called\e[0m" << std::endl;
	return (*this);
}

// Member functions
// field-line   = field-name ":" OWS field-value OWS
void HttpMessage::parseFieldLine(std::string &line, bool *headerRead, int errorCode)
{
	line = Validate::sanitize(line);
	if (line.empty())
	{
		*headerRead = true;
		return;
	}
	auto colon_pos = line.find(":");
	if (colon_pos == std::string::npos)
	{
		std::cout << line << std::endl;
		throw HttpError("Malformed header field: missing colon separator", errorCode);
	}
	if (colon_pos > 0 && std::isspace(line[colon_pos - 1]))
		throw HttpError("Malformed header field: whitespace before colon", errorCode);
	std::string key = Validate::headerName(line.substr(0, colon_pos));
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	std::string value = line.substr(colon_pos + 1);
	value.erase(0, value.find_first_not_of(" \t"));
	value.erase(value.find_last_not_of(" \t") + 1);
	_header[key] = value;
}

// Getters

const std::map<std::string, std::string> &HttpMessage::getHeader() const
{
	return _header;
}

// Setters
