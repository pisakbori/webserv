#include "Request.hpp"

// Constructor

// Default constructor
Request::Request()
{
	// std::cout << "\e[2mDefault constructor Request called\e[0m" << std::endl;
	_input = "";
	_bodySize = 0;
}

// Copy constructor
Request::Request(const Request &other)
{
	// std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Request::~Request()
{
	// // std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request &Request::operator=(const Request &other)
{
	// std::cout << "\e[2mAssign operator Request called\e[0m" << std::endl;
	if (this != &other)
	{
		_input = other._input;
		_method = other._method;
		_protocol = other._protocol;
		_uri = other._uri;
		_body = other._body;
		_bodySize = other._bodySize;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &os, const Request &req)
{
	std::cout << "Method: " << req.getMethod() << std::endl;
	std::cout << "Protocol: " << req.getProtocol() << std::endl;
	std::cout << "URI: " << req.getUri() << std::endl;
	for (auto it = req.getHeader().begin(); it != req.getHeader().end(); ++it)
	{
		std::cout << it->first << ": \"" << it->second << "\"" << std::endl;
	}
	std::cout << "Body: " << req.getBody() << std::endl;
	std::cout << "BodySize: " << req._bodySize << std::endl;
	return os;
}

// Member functions
void Request::matchHost(Connection *c)
{
	if (_header.find("HOST") == _header.end())
		throw HttpError("Missing Host", 400);
	const std::string &host = _header["HOST"];

	for (const int& i : c->getValidServers())
	{
		std::vector<std::string> server_names = c->getServ().at(i).get_server_name();
		if (std::find(server_names.begin(), server_names.end(), host) != server_names.end())
			c->setResponsibleServer(i);
	}
}

// request-line   = method SP request-target SP HTTP-version
void Request::parseRequestLine(std::string &line)
{
	Validate::sanitize(line);
	if (std::any_of(line.begin(), line.end(), [](char c)
		{ return std::isspace(c) && c != ' '; }))
	{
		throw HttpError("Invalid whitespace in request line: ", 400);
	}
	std::istringstream stream(line);

	stream >> _method >> _uri >> _protocol;
	if (_method.empty() || _uri.empty() || _protocol.empty())
		throw HttpError("Bad Request", 400);
	Validate::url(_uri);
	if (_protocol != "HTTP/1.1")
		throw HttpError(_protocol + " protocol not supported", 505);
}

void Request::parseContentLength(Connection *c, std::istringstream &stream)
{
	if (_header.find("TRANSFER-ENCODING") != _header.end())
		throw HttpError("Bad Request", 400);
	std::string	value;
	value = _header["CONTENT-LENGTH"];
	auto size = Validate::contentLength(value, c->getResponsibleServer().get_client_max_body_size());
	_bodySize = size;
	char ch;
	// TODO: if header read, only append to body would be ideal..
	_body = "";
	while (size > 0 && stream.get(ch))
	{
		_body.push_back(ch);
		size--;
	}
	if (size > 0)
		return;
	else if (stream.get(ch))
		throw HttpError("Request body is greater than CONTENT-LENGTH", 400);
	c->setState(Connection::REQ_READY);
}

void Request::parseRequest(Connection *c)
{
	bool headerRead = false;
	std::string line;
	std::istringstream stream(_input);
	if (_input.find("\n") == std::string::npos &&
		_input.find("\r\n") == std::string::npos)
		return;
	std::getline(stream, line);
	parseRequestLine(line);
	if (_input.find("\n\n") == std::string::npos &&
		_input.find("\r\n\r\n") == std::string::npos)
		return;
	while (!headerRead && std::getline(stream, line))
		parseFieldLine(line, &headerRead, 400);
	if (!headerRead)
		return;
	matchHost(c);
	if (_method == "GET" || _method == "HEAD" || _method == "DELETE")
		c->setState(Connection::REQ_READY);
	else if (_header.find("CONTENT-LENGTH") != _header.end())
		parseContentLength(c, stream);
	else if (_method == "POST")
	{
		// TODO: Chunked transfer encoding?
		throw HttpError("CONTENT-LENGTH or Transfer-Encoding header is required.", 411);
	}
	else
		c->setState(Connection::REQ_READY);
}

void Request::append(std::string const &str)
{
	_input.append(str);
}

// Getterss

std::string const &
Request::getMethod() const
{
	return _method;
}

std::string const &Request::getUri() const
{
	return _uri;
}

std::string const &Request::getProtocol() const
{
	return _protocol;
}

std::string const &Request::getBody() const
{
	return _body;
}

bool Request::hasConnectionClose() const
{
	auto it = _header.find("CONNECTION");
	std::string key = "";
	if (it != _header.end())
		key = it->second;
	std::transform(key.begin(), key.end(), key.begin(), ::toupper );
	return (key == "CLOSE");
}

const std::map<std::string, std::string> &Request::getHeader() const
{
	return _header;
}

// Setters
