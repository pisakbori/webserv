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
		_header = other._header;
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
// TODO:move to utils
std::string joinStrings(const std::vector<std::string> &vec, const std::string &delimiter)
{
	std::ostringstream oss;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		oss << vec[i];
		if (i != vec.size() - 1)
			oss << delimiter;
	}
	return oss.str();
}

void Request::validateAllowed(std::string uri, std::string method, const Server &serv)
{
	// TODO:where should i store location?:/
	auto location = serv.get_location(uri);
	auto allowed = location.get_allow();
	if (std::find(allowed.begin(), allowed.end(), method) == allowed.end())
	{
		auto err = HttpError(method + " method not allowed for " + uri, 405);
		err.setField("Allow", joinStrings(location.get_allow(), ", "));
		throw err;
	}
};

void Request::matchHost(Connection *c)
{
	std::string host = _header["HOST"];

	for (const int& i : c->getValidServers())
	{
		std::vector<std::string> server_names = c->getServ().at(i).get_server_name();
		if (std::find(server_names.begin(), server_names.end(), host) != server_names.end())
			c->setResponsibleServer(i);
	}
}

void Request::parseRequest(Connection *c)
{
	std::string line;
	bool headerRead = false;
	// <Method> <Request-URI> <HTTP-Version>
	std::istringstream stream(_input);
	if (_input.find("\n") == std::string::npos)
		return; // Return when EOF is encountered without a newline
	std::getline(stream, line);
	if (line.empty())
		throw HttpError("Bad Request", 400);
	line = Validate::sanitize(line);
	size_t separator1 = line.find(" ");
	_method = line.substr(0, separator1);
	size_t separator2 = line.find(" ", separator1 + 1);
	_uri =
		Validate::url(line.substr(separator1 + 1, separator2 - separator1 - 1));
	_protocol = line.substr(separator2 + 1, line.length() - separator2);
	if (_protocol != "HTTP/1.1")
		throw HttpError(_protocol + " protocol not supported", 505);
	// validateAllowed(_uri, _method, c->getServ());
	// if (_input.find("\n\n") == std::string::npos ||
	// 	_input.find("\r\n\r\n") == std::string::npos)
	// 	return; // Return when EOF is encountered without a newline
	// field-name: OWS field-value OWS
	while (std::getline(stream, line))
	{
		line = Validate::sanitize(line);
		if (line.empty())
		{
			headerRead = true;
			break;
		}
		else if (stream.eof())
			return;
		auto semi = std::find(line.begin(), line.end(), ':');
		// M question why not just line.find(":")?
		if (semi == line.end())
			throw HttpError("Malformed header field: missing colon separator", 400);
		auto end = std::find_if(line.begin(), semi, [](unsigned char c)
								{ return (c == ' ' || c == '\t'); });
		std::string key = Validate::headerName(std::string(line.begin(), end));
		std::transform(key.begin(), key.end(), key.begin(),
					   [](unsigned char c)
					   { return std::toupper(c); });
		// M question counldnt it just be "std::toupper" as the last parameter?
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
									  { return (c == ' ' || c == '\t'); });
		// M question maybe below is beter?
		// auto start = line.find_first_not_of(" \t", line.find(":"));
		end = std::find_if(start, line.end(), [](unsigned char c)
						   { return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end);
		_header.insert(_header.begin(),
					   std::pair<std::string, std::string>(key, value));
	}
	if (!headerRead)
		return;
	if (_header.find("HOST") == _header.end())
		throw HttpError("Missing Host", 400);
	else
		matchHost(c);
	if (_method == "GET" || _method == "HEAD")
	{
		c->setState(Connection::REQ_READY);
	}
	else if (_header.find("CONTENT-LENGTH") != _header.end())
	{
		try
		{
			auto size = std::stoll(_header["CONTENT-LENGTH"]);
			if (size < 0)
				throw HttpError("Invalid content length", 400);
			// TODO: throw 413 error if body too large
			_bodySize = size;
		}
		catch (const std::exception &e)
		{
			throw HttpError("Invalid content length", 400);
		}
		char ch;
		auto size = _bodySize;
		// TODO: if header read, only append to body would be ideal..
		_body = "";
		while (size > 0 && stream.get(ch))
		{
			_body.push_back(ch);
			size--;
		}
		if (size > 0)
			return;
		c->setState(Connection::REQ_READY);
		// we don't care about leftovers, even Chrome gave up HTTP pipelining.
	}
	else if (_method == "POST" || _method == "DELETE")
	{
		// TODO: Chunked transfer encoding?
		throw HttpError("Content-Length or Transfer-Encoding header is required.", 411);
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

const std::map<std::string, std::string> &Request::getHeader() const
{
	return _header;
}

// Setters
