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
    _statusCode = 200;
    _statusText = statuses[_statusCode];
    _header["Content-Type"] = "text/html";
}

// Parameterized constructor: autoindex with directory as arg
Response::Response(std::string dirPath, std::string url)
{
    std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    _statusCode = 200;
    _statusText = statuses[_statusCode];
    _header["Content-Type"] = "text/html";
    _body = generateAutoindex(dirPath, url);
}

Response::Response(const HttpError &err)
{
    std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    _statusCode = err.getCode();
    _statusText = statuses[_statusCode];
    _header["Content-Type"] = "text/html";
    for (const auto &pair : err.getExtraFields())
        _header[pair.first] = pair.second;
    _body = err.what();
    wrapInHtml();
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

void Response::appendToBody(std::string const &str)
{
    _body.append(str);
}

void Response::setContentType(std::string const &str)
{
    _header["Content-Type"] = str;
}

void Response::appendToHeader(std::string key, std::string value)
{
    _header[key] = value;
}

void Response::wrapInHtml()
{
    std::stringstream htmlContent;
    htmlContent << "<html>\n"
                << "<head><title>"
                << this->_statusCode << " " << this->_statusText
                << "</title></head>\n"
                << "<body>\n"
                << "<h1>"
                << this->_statusCode << " " << this->_statusText
                << "</h1>\n"
                << "<p>"
                << this->_body
                << "</p>\n"
                << "</body>\n"
                << "</html>";
    _body = htmlContent.str();
}

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
        content << it->first << ": " << it->second << std::endl;
    }
    content << std::endl;
    content << _body << std::endl;
    return content.str();
}
// Setters

std::string Response::generateAutoindex(std::string &dir, std::string &original)
{
    std::ostringstream html;

    html
        << "<!DOCTYPE html>\n"
        << "<html>\n"
        << "<head>\n"
        << "<title>Index of " << original << "</title>\n"
        << "</head>\n"
        << "<body>\n"
        << "<h1>Index of " << original << "</h1>\n"
        << "<ul>\n";

    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(dir))
        {
            std::string name = entry.path().filename().string() + (entry.is_directory() ? "/" : "");
            std::string url = entry.path();
            url = url.substr(dir.length());
            if (original.back() == '/' && !url.empty())
                original.pop_back();
            std::string combined = original + url;
            html << "<li><a href=\"" << combined << "\">" << name << "</a></li>\n";
            std::cout << "whole ahref=" << combined << "<<" << std::endl;
        }
        std::cout << Colors::RESET;
    }
    catch (const std::exception &e)
    {
        html << "<p>Error reading directory: " << e.what() << "</p>\n";
    }

    html << "</ul>\n"
         << "</body>\n"
         << "</html>\n";

    return html.str();
}
