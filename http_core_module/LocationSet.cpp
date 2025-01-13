/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationSet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:49:11 by mkijewsk          #+#    #+#             */
/*   Updated: 2025/01/13 16:16:23 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

void Location::populate_location(std::ifstream &infile)
{
	std::string directives[] =
		{
			"allow",
			"redirect",
			"root",
			"autoindex",
			"index"};
	void (Location::*fnptr[])(std::string) =
		{
			&Location::set_allow,
			&Location::set_redirect,
			&Location::set_root,
			&Location::set_autoindex,
			&Location::set_index};
	std::string line;
	while (std::getline(infile, line))
		this->set_location(directives, fnptr, line);
}

void Location::set_location(std::string *directives, void (Location::*fnptr[])(std::string), std::string directive)
{
	int i;
	int N = 5;

	i = 0;
	while (i < N && directive.find(directives[i]) == std::string::npos)
		i++;
	std::string arg = extract_parameters(directives[i], directive);
	if (i != N)
		(this->*fnptr[i])(arg);
}

void Location::set_uri(std::string uri)
{
	this->uri = extract_parameters("location", uri);
}

void Location::set_allow(std::string arg)
{
	std::istringstream iss(arg);
	allow.clear();
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "HEAD" || token == "GET" || token == "POST" || token == "DELETE")
			allow.push_back(token);
	}
}

void Location::set_redirect(std::string arg)
{
	std::istringstream iss(arg);
	std::string token;
	if (std::getline(iss, token, ' '))
	{
		try
		{
			redirect.first = std::stoi(token);
		}
		catch (const std::invalid_argument &)
		{
			redirect.first = 0;
			redirect.second = token;
			return;
		}
		if (std::getline(iss, token))
			redirect.second = token;
	}
}

void Location::set_root(std::string arg)
{
	root = arg;
}

void Location::set_autoindex(std::string arg)
{
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
}

void Location::set_index(std::string arg)
{
	index.pop_back();
	std::istringstream iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
		index.push_back(std::move(token));
}
