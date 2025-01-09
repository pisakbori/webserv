#ifndef RES_HPP
#define RES_HPP

#include "CommonIncludes.hpp"
#include "HttpError.hpp"

class Response
{
private:
	std::string _statusText;
	int _statusCode;
	std::string _body;
	std::map<std::string, std::string> _header;
	void wrapInHtml();

public:
	static std::map<int, std::string> statuses;
	// Constructor
	Response();
	// Parameterized constructor
	Response(std::string str);
	Response(const HttpError &err);

	// Copy constructor
	Response(const Response &);

	// Destructor
	~Response();

	// Overloads
	Response &operator=(const Response &);

	// Member functions
	void appendToHeader(std::string key, std::string value);
	void appendToBody(std::string const &str);
	void setContentType(std::string const &str);

	// Getters
	std::string getBody() const;
	std::string toString() const;
	// Setters
};

#endif
