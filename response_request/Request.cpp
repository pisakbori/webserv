#include "Request.hpp"

// Constructor

// Default constructor
Request::Request()
{
	// std::cout << "\e[2mDefault constructor Request called\e[0m" << std::endl;
	_input = "";
	_bodySize = 0;
	_chunk_size = -1;
}

// Copy constructor
Request::Request(const Request &other): HttpMessage(other)
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
		HttpMessage::operator=(other);
		_input = other._input;
		_query = other._query;
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
	std::cout << "QueryString: " << req.getQuery() << std::endl;
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
	std::string &host = _header["HOST"];
	host = host.substr(0, host.find(':'));
	for (const int& i : c->getValidServers())
	{
		std::vector<std::string> server_names = c->getServ().at(i).get_server_name();
		if (std::find(server_names.begin(), server_names.end(), host) != server_names.end())
			c->setResponsibleServer(i);
	}
}

void Request::extractQueryString()
{
	std::string	query;

	size_t question_pos = _uri.find('?');
	if (question_pos == std::string::npos)
		return ;
	_query = _uri.substr(question_pos + 1);
	_uri = _uri.substr(0, question_pos);
}

// request-line   = method SP request-target SP HTTP-version
void Request::parseRequestLine(std::string &line)
{
	Validate::sanitize(line);
	if (std::any_of(line.begin(), line.end(), [](char c)
		{ return std::isspace(c) && c != ' '; }))
	{
		throw HttpError("Invalid whitespace in request line", 400);
	}
	std::istringstream stream(line);

	stream >> _method >> _uri >> _protocol;
	if (_method.empty() || _uri.empty() || _protocol.empty())
		throw HttpError("Bad Request", 400);
	Validate::url(_uri);
	extractQueryString();
	if (_protocol != "HTTP/1.1" && _protocol != "HTTP/1.0")
		throw HttpError(_protocol + " protocol not supported", 505);
}

void Request::parseContentLength(Connection *c)
{
	if (_header.find("TRANSFER-ENCODING") != _header.end())
		throw HttpError("Bad Request", 400);
	std::string	value;
	value = _header["CONTENT-LENGTH"];
	auto size = Validate::contentLength(value, c->getResponsibleServer().get_client_max_body_size());
	if ((long long)_input.length() < size)
		return;
	if (size > c->getResponsibleServer().get_client_max_body_size())
		throw HttpError("Payload Too Large", 413);
	_body = "";
	_bodySize = size;
	_body.append(_input, 0, size);
	if ((long long)_input.size() > size)
		throw HttpError("Request body is greater than Content-length", 400);
	c->setState(Connection::REQ_READY);
}

void Request::parseTransferEncoding(Connection *c, const std::string &rclf)
{
	if (_header.find("CONTENT-LENGTH") != _header.end())
		throw HttpError("Bad Request", 400);
	if (_header["TRANSFER-ENCODING"] != "chunked")
		throw HttpError("Not Implemented", 501);
	while (!_input.empty())
	{
		if (_chunk_size == -1)
		{
			size_t pos = _input.find(rclf);
			if (pos == std::string::npos)
				return ;
			std::string line = _input.substr(0, pos);
			std::istringstream iss(line);
			if (!(iss >> std::hex >> _chunk_size))
				throw HttpError("Invalid Request", 400);
			_input.erase(0, pos + rclf.size());
			if (_chunk_size == 0)
				return (c->setState(Connection::REQ_READY));
		}
		size_t pos = _input.find(rclf);
		if (pos == std::string::npos)
			return ;
		if (pos != (size_t)_chunk_size)
			throw HttpError("Invalid Request", 400);
		_body.append(_input.substr(0, _chunk_size));
		_bodySize += _chunk_size;
		_input.erase(0, _chunk_size + rclf.size());
		_chunk_size = -1;
		if (_bodySize > (size_t)c->getResponsibleServer().get_client_max_body_size())
			throw HttpError("Payload Too Large", 413);
	}
}

void Request::parseHead(Connection *c)
{
	bool headerRead = false;
	std::string line;
	std::stringstream stream(_input);
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
	else
		c->setState(Connection::READING_REQ_BODY);
}

void Request::parseRequest(Connection *c)
{
	if (c->getState() == Connection::READING_REQ)
	{
		parseHead(c);
		if (c->getState() == Connection::READING_REQ_BODY)
		{
			if (_input.find("\r\n\r\n") != std::string::npos)
				_input.erase(0, _input.find("\r\n\r\n") + 4);
			else
				_input.erase(0, _input.find("\n\n") + 2);
		}
	}
	if (c->getState() == Connection::READING_REQ_BODY)
	{
		if (_header.find("CONTENT-LENGTH") != _header.end())
			parseContentLength(c);
		else if (_header.find("TRANSFER-ENCODING") != _header.end())
			parseTransferEncoding(c, "\r\n");
		else if (_method == "POST")
			throw HttpError("Content-Length or Transfer-Encoding header is required.", 411);
		else
			c->setState(Connection::REQ_READY);
	}
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

const std::string &Request::getQuery() const
{
	return _query;
}

// Setters
