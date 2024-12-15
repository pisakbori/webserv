/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:02:07 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/15 23:03:54 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

// # include <map>
# include <string>
# include <vector>
# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sstream>

/*
 * Server class stores information regarding server directive in the .conf
 * It's inspired by nginx directives: 
 * https://nginx.org/en/docs/http/ngx_http_core_module.html
 * #TODO
 * - [ ] (?) Inherit from the http directive
 * - [ ] Implement location member: vvv
 * https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
 * - [ ] Implement error_page member
 * - [ ] If cannot listen on default port 80 try 8080
 * - [x] Instead of checking everywhere for the trailing ';' and if it's starts with proper directrive do it somewhere else
*/

class Server
{

	private:

		std::string					host;
		unsigned short				port;
		std::vector<std::string>	server_name;
		// std::map<int, std::string>	error_page;
		size_t						client_max_body_size;
		// location

	public:

		Server();
		~Server();


		void						parse_listen(std::string directive);
		void						set_server_name(std::string directive);
		void						set_client_max_body_size(std::string directive);
		std::string					get_host( void ) const;
		unsigned short				get_port( void ) const;
		std::vector<std::string>	get_server_name( void ) const;
		size_t						get_client_max_body_size( void ) const;


};
std::ostream&	operator<<(std::ostream& os, const Server & serv);
std::string		extract_parameters(const std::string & name, const std::string & directive);

#endif
