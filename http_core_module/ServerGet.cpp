/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:37:36 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/19 17:44:49 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string					Server::get_host(void) const
{
	return host;
}

unsigned short				Server::get_port(void) const
{
	return port;
}

std::vector<std::string>	Server::get_server_name(void) const
{
	return server_name;
}

err_page_t					Server::get_error_page(void) const
{
	return error_page;
}

size_t						Server::get_client_max_body_size(void) const
{
	return client_max_body_size;
}
