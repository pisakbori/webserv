/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 22:03:33 by mkijewsk          #+#    #+#             */
/*   Updated: 2025/01/13 13:12:49 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() :
	host("0.0.0.0"),
	port(80),
	server_name(1),
	error_page(),
	client_max_body_size(1 << 20),
	location()
{
}

Server::Server(Server const & src) :
	host(src.host),
	port(src.port),
	server_name(src.server_name),
	error_page(src.error_page),
	client_max_body_size(src.client_max_body_size),
	location(src.location)
{
}

Server::~Server()
{
}

Server &		Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		this->host = rhs.host;
		this->port = rhs.port;
		this->server_name = rhs.server_name;
		this->error_page = rhs.error_page;
		this->client_max_body_size = rhs.client_max_body_size;
		this->location = rhs.location;
	}
	return *this;
}

std::ostream&	operator<<(std::ostream& os, const Server& server)
{
	os << "host: " << server.get_host() << std::endl;
	os << "port: " << server.get_port() << std::endl;
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
		arg = arg.substr(0, arg.size() - 1);
		return arg;
	}
	return "";
}
