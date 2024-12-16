/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 22:03:33 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/16 22:58:26 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() :
	host("0.0.0.0"),
	port(80),
	server_name(1),
	error_page(),
	client_max_body_size(1 << 20)
{
}

Server::Server(Server const & src) :
	host(src.host),
	port(src.port),
	server_name(src.server_name),
	error_page(src.error_page),
	client_max_body_size(src.client_max_body_size)
{
}

Server::~Server()
{
}

Server &	Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		this->host = rhs.host;
		this->port = rhs.port;
		this->server_name = rhs.server_name;
		this->error_page = rhs.error_page;
		this->client_max_body_size = rhs.client_max_body_size;
	}
	return *this;
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
