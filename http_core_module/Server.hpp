/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:02:07 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/17 21:07:35 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sstream>
# include "Location.hpp"

/*
 * Server class stores information regarding server directive in the .conf
 * It's inspired by nginx directives: 
 * https://nginx.org/en/docs/http/ngx_http_core_module.html
 * #TODO
 * - [ ] (?) Inherit from the http directive
 * - [ ] Check if port, host, error_code is in proper before assignment
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
		Location					location;

	public:

		Server();
		Server(Server const & src);
		~Server();
		Server &	operator=(Server const & rhs);

		void						parse_listen(std::string directive);
		void						set_server_name(std::string directive);
		void						set_error_page(std::string directive);
		void						set_client_max_body_size(std::string directive);
		std::string					get_host(void) const;
		unsigned short				get_port(void) const;
		std::vector<std::string>	get_server_name(void) const;
		err_page_t					get_error_page(void) const;
		size_t						get_client_max_body_size(void) const;
		Location &					get_location(void);


};
std::string		extract_parameters(const std::string & name, const std::string & directive);

#endif
