#include "Response.hpp"

std::map<int, std::string> Response::statuses = {
    {200, "OK"},
    {201, "Created"},
    {303, "See Other"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {411, "Length Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {500, "Internal Server Error"},
    {505, "HTTP Version Not Supported"}};

std::map<std::string, std::string> Response::mimeTypes = {
    {"html", "text/html"},
    {"htm", "text/html"},
    {"shtml", "text/html"},
    {"css", "text/css"},
    {"js", "application/javascript"},
    {"json", "application/json"},
    {"png", "image/png"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},
    {"svg", "image/svg+xml"},
    {"ico", "image/vnd.microsoft.icon"},
    {"pdf", "application/pdf"},
    {"zip", "application/zip"},
    {"txt", "text/plain"},
    {"xml", "application/xml"},
    {"mp4", "video/mp4"},
    {"webm", "video/webm"},
    {"ogg", "video/ogg"},
    {"mp3", "audio/mpeg"},
    {"wav", "audio/wav"},
    {"flac", "audio/flac"},
    {"aac", "audio/aac"},
    {"m4a", "audio/mp4"},
    {"woff", "font/woff"},
    {"woff2", "font/woff2"},
    {"ttf", "font/ttf"},
    {"otf", "font/otf"},
    {"eot", "application/vnd.ms-fontobject"},
    {"wasm", "application/wasm"},
};

// Constructor
Response::Response() : _body(std::make_unique<std::string>()), _fullContent(std::make_unique<std::string>())
{
    // std::cout << "\e[2mDefault constructor Response called\e[0m" << std::endl;
    setCode(200);
    setContentType("html");
}

// Parameterized constructor: autoindex with directory as arg
Response::Response(std::string dirPath, std::string url)
{
    // std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    setCode(200);
    setContentType("html");
    _body = std::make_unique<std::string>(generateAutoindex(dirPath, url));
    _fullContent = std::make_unique<std::string>();
}

Response::Response(const HttpError &err)
{
    // std::cout << "\e[2mParameterized constructor Response called\e[0m" << std::endl;
    setCode(err.getCode());
    setContentType("html");
    for (const auto &pair : err.getExtraFields())
        _header[pair.first] = pair.second;
    _body = std::make_unique<std::string>(err.what());
    wrapInHtml();
    _fullContent = std::make_unique<std::string>();
}

// Copy constructor
Response::Response(const Response &other)
{
    *this = other;
    // std::cout << "\e[2mCopy constructor Response called\e[0m" << std::endl;
}

// Destructor
Response::~Response()
{
    std::cout << "\e[2mDestructor Response called--------------------------------\e[0m" << std::endl;
}

// Overloads
Response &Response::operator=(const Response &other)
{
    // std::cout << "\e[2mAssign operator Response called\e[0m" << std::endl;
    if (this != &other)
    {
        _header = other._header;
        setCode(other._statusCode);
        _body = std::make_unique<std::string>(*other._body);
        _fullContent = std::make_unique<std::string>(*other._fullContent);
    }
    return *this;
}

// Member functions

void Response::appendToBody(std::string const &str)
{
    _body->append(str);
}

void Response::setContentType(std::string const &str)
{
    if (mimeTypes.find(str) != mimeTypes.end())
    {
        _header["Content-Type"] = mimeTypes[str];
    }
    else if (str.empty())
        _header["Content-Type"] = "application/octet-stream";
    else
        throw HttpError("Unsupported Media Type " + str, 415);
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
                << *_body
                << "</p>\n"
                << "</body>\n"
                << "</html>";
    *_body = htmlContent.str();
}

// Getters

const std::string Response::getContent(std::size_t from, std::size_t size) const
{
    return _fullContent->substr(from, size);
}

std::size_t Response::getSize() const
{
    return _fullContent->length();
}

void Response::setContent()
{
    std::stringstream content;
    content << "HTTP/1.1" << " " << _statusCode << " " << _statusText << std::endl;
    content << "Content-Length: " << _body->size() << std::endl;
    content << "Keep-Alive: timeout=" << KEEPALIVE_TIMEOUT << "s" << std::endl;
    if (_statusCode == 408)
        content << "Connection: close" << KEEPALIVE_TIMEOUT << "s" << std::endl;
    for (auto it = _header.begin(); it != _header.end(); ++it)
    {
        content << it->first << ": " << it->second << std::endl;
    }
    content << std::endl;
    _fullContent->append(content.str());
    _fullContent->append(*_body);
    _body.reset();
}
// Setters

std::string getMeta()
{
    std::string str = "<meta charset =\"UTF-8\"><meta name =\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    return str;
}

std::string getStyle()
{
    std::string str = "<style> body {font-family: Arial, sans-serif;margin: 0;height: 100vh; display: flex;justify-content: center; align-items: center; flex-direction: column; text-align: center;}h1{font - size : 3em;}ul { list-style: none; padding: 0; text-align: left; } ul li { margin: 10px 0; }</style>";
    return str;
}

std::string Response::generateAutoindex(std::string &dir, std::string &original)
{
    std::ostringstream html;

    html
        << "<!DOCTYPE html>\n"
        << "<html>\n"
        << "<head>\n"
        << getMeta()
        << "<title>Index of " << original << "</title>\n"
        << getStyle()
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
            if (original.back() == '/' && !url.empty() && url.front() == '/')
                original.pop_back();
            std::string combined = original + url;
            html << "<li><a href=\"" << combined << "\">" << name << "</a></li>\n";
        }
        std::cout << Colors::RESET;
    }
    catch (const std::exception &e)
    {
        html << "<p>Error reading directory: " << e.what() << "</p>\n";
    }

    html << "</ul>\n"
         << "<p><a href=\"/\">Return to Home Page</a></p>"
         << "</body>\n"
         << "</html>\n";

    return html.str();
}

void Response::setBody(const std::string &str)
{
    *_body = str;
}

void Response::setCode(int code)
{
    _statusCode = code;
    _statusText = statuses[_statusCode];
}
