#include "Response.hpp"

std::map<int, std::string> Response::statuses = {
    {200, "OK"},
    {303, "See Other"},
    {400, "Bad Request"},
    {404, "Not Found"},
    {408, "Request Timeout"},
    {405, "Method Not Allowed"},
    {414, "URI Too Long"},
    {500, "Internal Server Error"},
    {505, "HTTP Version Not Supported"}};

// Constructor
Response::Response()
{
    std::cout << "\e[2mDefault constructor Response called\e[0m" << std::endl;
}

// Parameterized constructor
Response::Response(std::string str)
{
    std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    _statusCode = 200;
    _statusText = statuses[_statusCode];
    _header["Content-Type"] = "text/plain";
    _body = str;
}

Response::Response(const HttpError &err)
{
    std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    _statusCode = err.code();
    _statusText = statuses[_statusCode];
    _body = err.what();
    _header["Content-Type"] = "text/plain";
}

// Copy constructor
Response::Response(const Response &other)
{
    *this = other;
    std::cout << "\e[2mCopy constructor Response called\e[0m" << std::endl;
}

// Destructor
Response::~Response()
{
    std::cout << "\e[2mDestructor Response called\e[0m" << std::endl;
}

// Overloads
Response &Response::operator=(const Response &other)
{
    std::cout << "\e[2mAssign operator Response called\e[0m" << std::endl;
    if (this != &other)
    {
        _header = other._header;
        _statusCode = other._statusCode;
        _statusText = other._statusText;
        _body = other._body;
    }
    return *this;
}

// Member functions

// Getters
std::string Response::getBody() const
{
    return _body;
}

std::string Response::toString() const
{
    std::stringstream content;
    content << "HTTP/1.1" << " " << _statusCode << " " << _statusText << std::endl;
    content << "Content-Length: " << _body.size() << std::endl;
    content << "Keep-Alive: timeout=" << KEEPALIVE_TIMEOUT << "s" << std::endl;
    if (_statusCode == 408)
        content << "Connection: close" << KEEPALIVE_TIMEOUT << "s" << std::endl;
    for (auto it = _header.begin(); it != _header.end(); ++it)
    {
        content << it->first << ": \"" << it->second << "\"" << std::endl;
    }
    content << std::endl;
    content << _body << std::endl;
    return content.str();
}
// Setters
