#include "Validate.hpp"

#include <iostream>

// Constructor
Validate::Validate()
{
	std::cout << "\e[2mDefault constructor Validate called\e[0m" << std::endl;
}

// Parameterized constructor
// Validate::Validate()
// {
// 	std::cout << "\e[2mParameterized constructor Validate called\e[0m" <<
// std::endl;
// }

// Copy constructor
Validate::Validate(const Validate& other)
{
	(void) other;
	std::cout << "\e[2mCopy constructor Validate called\e[0m" << std::endl;
}

// Destructor
Validate::~Validate()
{
	std::cout << "\e[2mDestructor Validate called\e[0m" << std::endl;
}

// Overloads
Validate& Validate::operator=(const Validate& other)
{
	(void) other;
	std::cout << "\e[2mAssignation operator Validate called\e[0m" << std::endl;
	return (*this);
}

// Member functions
std::string Validate::url(std::string str)
{
	std::stringstream msg;
	size_t i = 0;

	std::string invalid = " ()#<>{}|\\^~[]`";
	while (i < str.length())
	{
		if (invalid.find(str[i]) != std::string::npos)
		{
			msg << "Bad Request: Invalid character in url at position " << i
			    << " " << static_cast<int>(str[i]);
			throw HttpError(msg.str(), 400);
		}
		i++;
	}
	return str;
}

std::string Validate::sanitize(std::string& str)
{
	std::stringstream msg;

	if (!str.empty() && str.back() == '\r')
		str.pop_back();
	size_t i = 0;
	while (i < str.length())
	{
		if ((i < str.length() - 1) && (str[i] == '\r' && str[i + 1] == '\n'))
			i++;
		else if ((str[i] <= 31 && str[i] >= 0) || str[i] == 127)
		{
			msg << "Bad Request: Control character found at position " << i
			    << " " << static_cast<int>(str[i]);
			throw HttpError(msg.str(), 400);
		}
		i++;
	}
	return str;
}

std::string Validate::headerName(std::string str)
{
	size_t i = 0;

	if (!str.length())
		throw HttpError("Bad Request: Empty header name", 400);
	while (i < str.length())
	{
		if (!(isalnum(str[i]) || str[i] == '-' || str[i] == '_'))
		{
			std::cout << ">" << str << "<" << std::endl;
			throw HttpError("Bad Request: Invalid character in header name",
			                400);
		}
		i++;
	}
	return str;
}

// Getters

// Setters
