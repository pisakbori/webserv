/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 20:36:50 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/16 22:27:37 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Server::Location::Location() :
	allow(),
	redirect(),
	root("html"),
	autoindex(false),
	index()
{
	allow.push_back("GET");
	allow.push_back("HEAD");
	allow.push_back("POST");
	index.push_back("index.html");
}

Server::Location::~Location()
{
}

void	Server::Location::set_allow(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("allow", directive);
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
	{
		std::vector<std::string>::iterator	position;
		position = std::find(allow.begin(), allow.end(), token);
		if (position != allow.end())
			allow.erase(position);
	}
}

void	Server::Location::set_redirect(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("redirect", directive);
	std::istringstream	iss(arg);
	std::string	token;
	if (std::getline(iss, token, ' '))
	{
		try
		{
			redirect.first = std::stoi(token);
		}
		catch(const std::invalid_argument&)
		{
			redirect.first = 0;
			redirect.second = token;
			return;
		}
		if (std::getline(iss, token))
			redirect.second = token;
	}
}

void	Server::Location::set_root(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("root", directive);
	root = arg;
}

void	Server::Location::set_autoindex(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("autoindex", directive);
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
}

void			Server::Location::set_index(std::string directive)
{
	std::string			arg;

	arg = extract_parameters("index", directive);
	index.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        index.push_back(std::move(token));
}

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
