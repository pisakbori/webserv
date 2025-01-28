#include "Cgi.hpp"

#include <iostream>

// Constructor
Cgi::Cgi()
{
	// std::cout << "\e[2mDefault constructor Cgi called\e[0m" << std::endl;
}

// Parameterized constructor
// Cgi::Cgi()
// {
// 	std::cout << "\e[2mParameterized constructor Cgi called\e[0m" << std::endl;
// }

// Copy constructor
Cgi::Cgi(const Cgi &other)
{
	// std::cout << "\e[2mCopy constructor Cgi called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Cgi::~Cgi()
{
	// std::cout << "\e[2mDestructor Cgi called\e[0m" << std::endl;
}

// Overloads
Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
		(void)other;
	}
	// std::cout << "\e[2mAssignation operator Cgi called\e[0m" << std::endl;
	return (*this);
}

// Member functions

// Getters

// Setters
