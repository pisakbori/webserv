/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:37:36 by mkijewsk          #+#    #+#             */
/*   Updated: 2025/01/09 14:18:49 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string Server::get_host(void) const
{
	return host;
}

unsigned short Server::get_port(void) const
{
	return port;
}

std::vector<std::string> Server::get_server_name(void) const
{
	return server_name;
}

err_page_t Server::get_error_page(void) const
{
	return error_page;
}

size_t Server::get_client_max_body_size(void) const
{
	return client_max_body_size;
}

int Server::getListenFd() const
{
	return _listenFd;
}

// TODO: match location based on uri
const Location &Server::get_location(std::string uri, std::string &route) const
{
	std::string root = location.get_root();
	if (location.get_uri() == uri)
		route = root;
	else if (uri.find(location.get_uri()) == 0)
		route = root + "/" + uri.substr(location.get_uri().length());
	else
		throw HttpError("Oh no! " + uri + " not found.", 404);
	return location;
}