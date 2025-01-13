/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:02:07 by mkijewsk          #+#    #+#             */
/*   Updated: 2025/01/13 13:13:11 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <iostream>
# include <fstream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sstream>
# include "Location.hpp"

/*
 * Server class stores information regarding server directive in the .conf
 * It's inspired by nginx directives: 
 * https://nginx.org/en/docs/http/ngx_http_core_module.html
 * #TODO
*/

typedef struct	err_page_s
{
	std::vector<int>	code;
	int					overwrite;
	std::string			uri;
}	err_page_t;

class Server
{

	private:

		std::string					host;
		unsigned short				port;
		std::vector<std::string>	server_name;
		err_page_t					error_page;
		size_t						client_max_body_size;
		void						set_server(std::string directive);
		void						parse_listen(std::string arg);
		void						set_host(std::string arg);
		void						set_port(std::string arg);
		void						set_server_name(std::string arg);
		void						set_error_page(std::string arg);
		void						set_client_max_body_size(std::string arg);

	public:

		Server();
		Server(Server const & src);
		~Server();
		Server &	operator=(Server const & rhs);

		void						populate_server(std::ifstream & infile);
		std::string					get_host(void) const;
		unsigned short				get_port(void) const;
		std::vector<std::string>	get_server_name(void) const;
		err_page_t					get_error_page(void) const;
		size_t						get_client_max_body_size(void) const;
		std::vector<Location>		location;


};
std::ostream &	operator<<(std::ostream & os, const Server & server);
std::string		extract_parameters(
	const std::string & name,
	const std::string & directive
	);

#endif
