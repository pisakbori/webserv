#include "Response.hpp"

#include <iostream>

// Constructor
Response::Response() : 	 statuses{
        {200, "OK"},
        {303, "See Other"},
        {400, "Bad Request"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {414, "URI Too Long"},
        {500, "Internal Server Error"}
    }
{
	std::cout << "\e[2mDefault constructor Response called\e[0m" << std::endl;
}

// Parameterized constructor
// Response::Response()
// {
// 	std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
// }

// Copy constructor
Response::Response(const Response &other)
{
	(void)other;
	std::cout << "\e[2mCopy constructor Response called\e[0m" << std::endl;
}

// Destructor
Response::~Response()
{
	std::cout << "\e[2mDestructor Response called\e[0m" << std::endl;
}

// Overloads
Response	&Response::operator=(const Response &other)
{
	(void)other;
	std::cout << "\e[2mAssignation operator Response called\e[0m" << std::endl;
	return (*this);
}

// Member functions

// Getters

// Setters
