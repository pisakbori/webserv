/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:38:59 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/19 17:48:18 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void			Server::populate_server(std::ifstream & infile)
{
	std::string		directives[] =
	{
		"listen",
		"server_name",
		"error_page",
		"client_max_body_size"
	};
	void	(Server::*fnptr[])( std::string ) =
	{
		&Server::parse_listen,
		&Server::set_server_name,
		&Server::set_error_page,
		&Server::set_client_max_body_size
	};
	std::string		line;

	while (std::getline(infile, line))
	{
		if (line.find("location") != std::string::npos)
		{
			this->location.set_uri(line);	
			this->location.populate_location(infile);
		}
		this->set_server(directives, fnptr, line);
	}
}

void			Server::set_server(std::string *directives, void (Server::*fnptr[])( std::string ), std::string directive)
{
	int	i;
	int	N = 4;

	i = 0;
	while (i < N && directive.find(directives[i]) == std::string::npos)
		i++;
	std::string arg = extract_parameters(directives[i], directive);
	if (i != N)
		(this->*fnptr[i])(arg);
}

void			Server::parse_listen(std::string arg)
{
	const size_t	colon_pos = arg.rfind(':');
	if (colon_pos != std::string::npos &&
		(arg.find(']') < colon_pos ||
		arg.find(']') == std::string::npos))
	{
		host = arg.substr(0, colon_pos);
		port = std::stoi(arg.substr(colon_pos + 1));
	}
	else if (arg.find_first_not_of("0123456789") == std::string::npos)
		port = std::stoi(arg);
	else
		host = arg;
}

void			Server::set_server_name(std::string arg)
{
	server_name.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        server_name.push_back(std::move(token));
}

void			Server::set_error_page(std::string arg)
{
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "=")
			error_page.overwrite = 0;
		else if (token.size() == 3 && std::isdigit(token[0]) &&
			std::isdigit(token[1]) && std::isdigit(token[2]))
		{
			error_page.code.push_back(std::stoi(token));
			error_page.overwrite = 200;
		}
		else if (token.size() == 4 && token.front() == '=' &&
			std::isdigit(token[1]) && std::isdigit(token[2]) &&
			std::isdigit(token[3]))
			error_page.overwrite = std::stoi(token.substr(1));
		else
			error_page.uri = token;
	}
}

void			Server::set_client_max_body_size(std::string arg)
{
	if (arg.substr(0, arg.size() - 2).find_first_not_of("0123456789") == std::string::npos)
	{
		if (std::tolower(arg.back()) == 'k')
			client_max_body_size = (std::stoi(arg) << 10);
		else if (std::tolower(arg.back()) == 'm')
			client_max_body_size = (std::stoi(arg) << 20);
		else if (std::isdigit(arg.back()))
			client_max_body_size = std::stoi(arg);
	}
	else
		std::cerr << "Invalid input" << std::endl;
}
