#include "Server.hpp"

std::string		extract_parameters(
	const std::string & name,
	const std::string & directive
	)
{
	std::string	arg;
	if (directive.find(name) != std::string::npos
		&& directive.find(' ') != std::string::npos
		&& directive.back() == ';')
	{
		arg = directive.substr(directive.find(' ') + 1);
		arg = arg.substr(0, arg.find_last_not_of("; ") + 1);
		return arg;
	}
	return "";
}

// Constructor
Server::Server() :
	listen(),
	server_name(1),
	error_page(),
	client_max_body_size(1 << 20),
	location()
{
	std::cout << "\e[2mServer default constructor\e[0m" << Colors::RESET << std::endl;
}

// Copy constructor
Server::Server(Server const & src) :
	listen(src.listen),
	server_name(src.server_name),
	error_page(src.error_page),
	client_max_body_size(src.client_max_body_size),
	location(src.location)
{
	std::cout << "\e[2mServer copy constructor\e[0m" << Colors::RESET << std::endl;
}

// Destructor
Server::~Server()
{
	std::cout << "\e[2mServer destructor\e[0m" << Colors::RESET << std::endl;
}

// Overloads
Server &		Server::operator=(Server const & rhs)
{
	std::cout << "\e[2mServer equal operator" << Colors::RESET << std::endl;
	if (this != &rhs)
	{
		this->listen = rhs.listen;
		this->server_name = rhs.server_name;
		this->error_page = rhs.error_page;
		this->client_max_body_size = rhs.client_max_body_size;
		this->location = rhs.location;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &os, const Server &server)
{
	// os << server.get_listen();
	os << "server_name: ";
	std::vector<std::string> v(server.get_server_name());
	for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
		os << *i << ' ';
	os << std::endl;
	os << "error_page: " << std::endl;
	std::vector<int> vint(server.get_error_page().code);
	os << "  code(s): ";
	for (size_t i = 0; i < vint.size(); ++i)
		os << vint[i] << ' ';
	os << std::endl;
	os << "  overwrite: " << server.get_error_page().overwrite << std::endl;
	os << "  uri: " << server.get_error_page().uri << std::endl;
	os << "client_max_body_size: " << server.get_client_max_body_size() << std::endl;
	std::vector<Location> l(server.location);
	for (size_t i = 0; i < l.size(); ++i)
  		os << l[i] << std::endl;
	return os;
}

void Server::startListening(void)
{
	listen.startListening();
};

void Server::stopListening(void)
{
	listen.stopListening();
}
