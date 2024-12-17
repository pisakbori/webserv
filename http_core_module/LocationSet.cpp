/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationSet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:49:11 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/17 21:19:32 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

void	Location::set_allow(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("allow", directive);
	std::istringstream	iss(arg);
	allow.clear();
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "HEAD" || token == "GET" || token == "POST")
			allow.push_back(token);
	}
}

void	Location::set_redirect(std::string directive)
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

void	Location::set_root(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("root", directive);
	root = arg;
}

void	Location::set_autoindex(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("autoindex", directive);
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
}

void	Location::set_index(std::string directive)
{
	std::string			arg;

	arg = extract_parameters("index", directive);
	index.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        index.push_back(std::move(token));
}
