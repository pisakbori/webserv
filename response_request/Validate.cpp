#include "Validate.hpp"

// Constructor
Validate::Validate()
{
	// std::cout << "\e[2mDefault constructor Validate called\e[0m" << std::endl;
}

// Parameterized constructor
// Validate::Validate()
// {
// 	// std::cout << "\e[2mParameterized constructor Validate called\e[0m" <<
// std::endl;
// }

// Copy constructor
Validate::Validate(const Validate &other)
{
	(void)other;
	// std::cout << "\e[2mCopy constructor Validate called\e[0m" << std::endl;
}

// Destructor
Validate::~Validate()
{
	// std::cout << "\e[2mDestructor Validate called\e[0m" << std::endl;
}

// Overloads
Validate &Validate::operator=(const Validate &other)
{
	(void)other;
	// std::cout << "\e[2mAssign operator Validate called\e[0m" << std::endl;
	return (*this);
}

// Member functions
std::string Validate::url(std::string str)
{
	std::stringstream msg;
	size_t i = 0;

	std::string invalid = " ()#<>{}|\\^~[]`";
	if (str.length() > 8000)
		throw HttpError("URI Too Long", 414);
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

std::string Validate::sanitize(std::string &str)
{
	std::stringstream msg;

	if (!str.empty() && str.back() == '\r')
	{
		str.pop_back();
	}
	size_t i = 0;
	while (i < str.length())
	{
		if ((i < str.length() - 1) && (str[i] == '\r' && str[i + 1] == '\n'))
			i++;
		else if (std::iscntrl(str[i]))
		{
			msg << "Control character found at position " << i
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

	if (str.empty())
		throw HttpError("Empty header name", 400);
	while (i < str.length())
	{
		if (!(isalnum(str[i]) || str[i] == '-' || str[i] == '_'))
		{
			std::cout << ">" << str << "<" << std::endl;
			throw HttpError("Invalid character in header name",
							400);
		}
		i++;
	}
	return str;
}

long long Validate::contentLength(std::string value, long long max)
{
	if (value.find_first_not_of("0123456789") != std::string::npos)
		throw HttpError("Content-length contains nondigit", 400);
	auto size = std::stoll(value);
	if (size > max)
		throw HttpError("Payload Too Large", 413);
	return size;
}
// Getters

// Setters
