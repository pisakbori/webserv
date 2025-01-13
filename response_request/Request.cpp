#include "Request.hpp"

// Constructor

// Default constructor
Request::Request()
{
	std::cout << "\e[2mDefault constructor Request called\e[0m" << std::endl;
	_input = "";
}

// Copy constructor
Request::Request(const Request &other)
{
	std::cout << "\e[2mCopy constructor Request called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Request::~Request()
{
	// std::cout << "\e[2mDestructor Request called\e[0m" << std::endl;
}

// Overloads
Request &Request::operator=(const Request &other)
{
	std::cout << "\e[2mAssign operator Request called\e[0m" << std::endl;
	if (this != &other)
	{
		_input = other._input;
		_header = other._header;
		_method = other._method;
		_protocol = other._protocol;
		_uri = other._uri;
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

void Request::parseRequest(Connection *c)
{
	std::string line;
	// <Method> <Request-URI> <HTTP-Version>
	std::istringstream _stream(_input);
	std::getline(_stream, line);
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
	validateAllowed(_uri, _method, c->getServ());
	// field-name: OWS field-value OWS
	while (std::getline(_stream, line))
	{
		line = Validate::sanitize(line);
		if (line.empty())
		{
			c->setState(Connection::REQ_READY);
			return;
		}
		auto semi = std::find(line.begin(), line.end(), ':');
		if (semi == line.end())
			throw HttpError("Malformed header field: missing colon separator", 400);
		auto end = std::find_if(line.begin(), semi, [](unsigned char c)
								{ return (c == ' ' || c == '\t'); });
		std::string key = Validate::headerName(std::string(line.begin(), end));
		std::transform(key.begin(), key.end(), key.begin(),
					   [](unsigned char c)
					   { return std::toupper(c); });
		auto start = std::find_if_not(semi + 1, line.end(), [](unsigned char c)
									  { return (c == ' ' || c == '\t'); });
		end = std::find_if(start, line.end(), [](unsigned char c)
						   { return (c == ' ' || c == '\t'); });
		std::string value = std::string(start, end);
		_header.insert(_header.begin(),
					   std::pair<std::string, std::string>(key, value));
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

const std::map<std::string, std::string> &Request::getHeader() const
{
	return _header;
}

// Setters
