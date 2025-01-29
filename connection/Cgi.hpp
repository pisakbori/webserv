#ifndef CGI_HPP
#define CGI_HPP

#include "Connection.hpp"
#include "Request.hpp"
#include <signal.h>

class Request;

class Cgi
{
private:
public:
	pid_t _cgiPid;
	std::vector<std::string> _cgiEnv;
	int _cgi2parent[2];
	int _parent2cgi[2];
	std::string _cgiResult;

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
		void startCGIprocess(const Request *req, std::filesystem::path path, const Server &server);
		void setCgiEnv(const Request *req, const Server &server);
		void killCgi();
		// Getters

		// Setters
		void init();
};

#endif
