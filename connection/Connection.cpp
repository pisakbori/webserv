#include "Connection.hpp"

#include <iostream>

// Constructor
Connection::Connection()
{
	std::cout << "\e[2mDefault constructor Connection called\e[0m" << std::endl;
}

// Parameterized constructor
// Connection::Connection()
// {
// 	std::cout << "\e[2mParameterized constructor Connection called\e[0m" << std::endl;
// }

// Copy constructor
Connection::Connection(const Connection &other)
{
	(void)other;
	std::cout << "\e[2mCopy constructor Connection called\e[0m" << std::endl;
}

// Destructor
Connection::~Connection()
{
	std::cout << "\e[2mDestructor Connection called\e[0m" << std::endl;
}

// Overloads
Connection &Connection::operator=(const Connection &other)
{
	(void)other;
	std::cout << "\e[2mAssignation operator Connection called\e[0m" << std::endl;
	return (*this);
}

// Member functions

// Getters

// Setters
