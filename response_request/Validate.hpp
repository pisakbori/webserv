#ifndef Validate_HPP
#define Validate_HPP

#include "HttpError.hpp"
#include <sstream>
#include <iostream>

class Validate
{
private:
public:
	// Constructor
	Validate();

	// Parameterized constructor

	// Copy constructor
	Validate(const Validate &);

	// Destructor
	~Validate();

	// Overloads
	Validate &operator=(const Validate &);

	// Member functions
	static std::string url(std::string str);
	static std::string sanitize(std::string &str);
	static std::string headerName(std::string str);
	// Getters

	// Setters
};

#endif
