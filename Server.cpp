/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 22:03:33 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/15 23:11:08 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
 * default server configuration
 * only listen directive is required
*/
Server::Server() :
	host("0.0.0.0"),
	port(80),
	server_name(1),
	// error_page(NULL),
	client_max_body_size(1 << 20)
{
}

Server::~Server()
{
}

std::string		extract_parameters(const std::string & name, const std::string & directive)
{
	std::string	arg;

	if (directive.find(name) == 0)
	{
		arg = directive.substr(directive.find(' ') + 1);
		arg = arg.substr(0, arg.size() - 1);
		return arg;
	}
	return "";
}

void			Server::parse_listen(std::string directive)
{
	std::string		arg;

	arg = extract_parameters("listen", directive);
	const size_t	colon_pos = arg.rfind(':');
	if (colon_pos != std::string::npos &&
		(arg.find(']') < colon_pos ||
		arg.find(']') == std::string::npos))
	{
		host = arg.substr(0, colon_pos);
		port = std::stoi(arg.substr(colon_pos + 1));
	}
	else if (arg.find_first_not_of("0123456789") == std::string::npos)
		port = std::stoi(arg);
	else
		host = arg;
}

void			Server::set_server_name(std::string directive)
{
	std::string			arg;

	arg = extract_parameters("server_name", directive);
	server_name.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        server_name.push_back(std::move(token));
}

void			Server::set_client_max_body_size(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("client_max_body_size", directive);
	if (arg.substr(0, arg.size() - 2).find_first_not_of("0123456789") == std::string::npos)
	{
		if (std::tolower(arg.back()) == 'k')
			client_max_body_size = (std::stoi(arg) << 10);
		else if (std::tolower(arg.back()) == 'm')
			client_max_body_size = (std::stoi(arg) << 20);
		else if (std::isdigit(arg.back()))
			client_max_body_size = std::stoi(arg);
	}
	else
		std::cerr << "Invalid input" << std::endl;
}

std::string		Server::get_host( void ) const
{
	return host;
}

unsigned short	Server::get_port( void ) const
{
	return port;
}

std::vector<std::string>	Server::get_server_name( void ) const
{
	return server_name;
}

size_t						Server::get_client_max_body_size( void ) const
{
	return client_max_body_size;
}

std::ostream&	operator<<(std::ostream& os, const Server& serv)
{
	os << serv.get_host() << ":" << serv.get_port();
	return os;
}
