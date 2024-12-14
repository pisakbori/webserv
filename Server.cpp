/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 22:03:33 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/14 23:18:52 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
	default server configuration
	only listen directive is required
*/
Server::Server() :
	host("0.0.0.0"),
	ports(new unsigned short(80)),
	server_name(new std::string()),
	is_default(true), // should be true if this is the first server in the conf
	// error_page(NULL),
	client_max_body_size(1'000'000)
{
}

Server::~Server()
{
	delete ports;
	delete server_name;
}