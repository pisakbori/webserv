/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 22:03:33 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/15 20:49:33 by mkijewsk         ###   ########.fr       */
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
	server_name(1)
	// is_default(true), // should be true if this is the first server in the conf
	// error_page(NULL),
	// client_max_body_size(1'000'000)
{
}

Server::~Server()
{
}

void			Server::parse_listen(std::string directive)
{
	std::string		argument;

	if (directive.rfind("listen ") != 0 || directive.back() != ';')
		std::cerr << "Invalid listen directive" << std::endl;
	argument = directive.substr(directive.find(' ') + 1);
	argument = argument.substr(0, argument.size() - 1);
	const size_t	colon_pos = argument.rfind(':');
	if (colon_pos != std::string::npos &&
		(argument.find(']') < colon_pos ||
		argument.find(']') == std::string::npos))
	{
		host = argument.substr(0, colon_pos);
		port = std::stoi(argument.substr(colon_pos + 1));
	}
	else if (argument.find_first_not_of("0123456789") == std::string::npos)
		port = std::stoi(argument);
	else
		host = argument;
}

void			Server::set_server_name(std::string directive)
{
	std::string			argument;

	if (directive.rfind("server_name ") != 0 || directive.back() != ';')
		std::cerr << "Invalid server_name directive" << std::endl;
	argument = directive.substr(directive.find(' ') + 1);
	argument = argument.substr(0, argument.size() - 1);
	server_name.pop_back();
	std::istringstream	iss(argument);
	for (std::string token; std::getline(iss, token, ' ');)
        server_name.push_back(std::move(token));
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

std::ostream&	operator<<(std::ostream& os, const Server& serv)
{
	os << serv.get_host() << ":" << serv.get_port();
	return os;
}
