#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "CommonIncludes.hpp"

class Response
{
	private:
		
	public:
		const std::map<int, std::string> statuses;
	// Constructor
		Response();
	// Parameterized constructor
		
	// Copy constructor
		Response(const Response &);
		
	// Destructor
		~Response();
		
	// Overloads
		Response	&operator=(const Response &);
		
	// Member functions
		
	// Getters
		
	// Setters
		
};

#endif
