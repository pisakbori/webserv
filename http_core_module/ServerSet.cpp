/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 22:38:59 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/16 22:39:26 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void			Server::parse_listen(std::string directive)
{
	std::string		arg;

	arg = extract_parameters("listen", directive);
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

void			Server::set_server_name(std::string directive)
{
	std::string			arg;

	arg = extract_parameters("server_name", directive);
	server_name.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        server_name.push_back(std::move(token));
}

void			Server::set_error_page(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("error_page", directive);
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

void			Server::set_client_max_body_size(std::string directive)
{
	std::string	arg;

	arg = extract_parameters("client_max_body_size", directive);
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
