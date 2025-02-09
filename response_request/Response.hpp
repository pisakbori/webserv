#ifndef RES_HPP
#define RES_HPP

#include "Location.hpp"
#include "HttpMessage.hpp"
#include <climits>

class Response : public HttpMessage
{
private:
	std::string _statusText;
	int _statusCode;
	std::unique_ptr<std::string> _body;
	std::unique_ptr<std::string> _fullContent;
	void wrapInHtml();

public:
	static std::map<int, std::string> statuses;
	static std::map<std::string, std::string> mimeTypes;

	// Constructor
	Response();
	// Parameterized constructor
	Response(std::filesystem::path dirPath, std::filesystem::path url);
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
	std::string generateAutoindex(std::filesystem::path &dir, std::filesystem::path &original);

	// Getters
	const std::string getContent(std::size_t from, std::size_t to) const;
	std::size_t getSize() const;
	int getCode() const;
	// Setters
	void setContent(bool withBody = true);
	void setCGIContent(std::string cgiOutput);
	void setBody(const std::string &str);
	void setCode(int code);
};

std::ostream &operator<<(std::ostream &os, const Response &res);

#endif
