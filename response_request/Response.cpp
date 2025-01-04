#include "Response.hpp"

const std::map<int, std::string> Response::statuses = {
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
    _body = str;
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
        _method = other._method;
        _protocol = other._protocol;
        _uri = other._uri;
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

// Setters
