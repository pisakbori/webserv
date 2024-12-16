/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationGet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:48:52 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/16 22:49:01 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

std::vector<std::string>	Server::Location::get_allow(void) const
{
	return allow;
}

std::pair<int, std::string>	Server::Location::get_redirect(void) const
{
	return redirect;
}

std::string					Server::Location::get_root(void) const
{
	return root;
}

bool						Server::Location::get_autoindex(void) const
{
	return autoindex;
}

std::vector<std::string>	Server::Location::get_index(void) const
{
	return index;
}
