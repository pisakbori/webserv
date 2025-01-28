#ifndef CGI_HPP
#define CGI_HPP

#include "Connection.hpp"
#include "Request.hpp"

class Request;

class Cgi
{
private:
public:
	pid_t _cgiPid;
	std::vector<std::string> _cgiEnv;
	int _cgi2parent[2];
	int _parent2cgi[2];
	// Constructor
	Cgi();

	// Parameterized constructor
		
	// Copy constructor
		Cgi(const Cgi &);
		
	// Destructor
		~Cgi();
		
	// Overloads
		Cgi	&operator=(const Cgi &);
		
	// Member functions
		void startCGIprocess(const Request *req, std::filesystem::path path, const Location &location);
		void setCgiEnv(const Request *req, std::string cgiPath);

		// Getters

		// Setters
		void setPipes();
};

#endif
