#ifndef Validate_HPP
#define Validate_HPP

#include <string>
#include "HttpError.hpp"

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
	static void url(std::string str);
	static void sanitize(std::string str);
	static void headerName(std::string str);
	// Getters

	// Setters
};

#endif
