/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationGet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:48:52 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/17 22:03:26 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

std::string					Location::get_uri(void) const
{
	return uri;
}

std::vector<std::string>	Location::get_allow(void) const
{
	return allow;
}

std::pair<int, std::string>	Location::get_redirect(void) const
{
	return redirect;
}

std::string					Location::get_root(void) const
{
	return root;
}

bool						Location::get_autoindex(void) const
{
	return autoindex;
}

std::vector<std::string>	Location::get_index(void) const
{
	return index;
}
