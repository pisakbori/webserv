/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:02:07 by mkijewsk          #+#    #+#             */
/*   Updated: 2025/01/01 17:26:56 by bpisak-l         ###   ########.fr       */
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
# include "CommonIncludes.hpp"

/*
 * Server class stores information regarding server directive in the .conf
 * It's inspired by nginx directives: 
 * https://nginx.org/en/docs/http/ngx_http_core_module.html
 * #TODO
 * - [ ] Check if port, host, error_code is in proper before assignment
 * - [ ] Server can have multiple locations
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
		struct 						sockaddr_in _serverAddr;
		int 						_listenFd;

	public:

		Server();
		Server(Server const & src);
		~Server();
		Server &	operator=(Server const & rhs);

		void						populate_server(std::ifstream & infile);
		void						set_server(std::string *directives, void (Server::*fnptr[])( std::string ), std::string directive);
		void						parse_listen(std::string arg);
		void						set_server_name(std::string arg);
		void						set_error_page(std::string arg);
		void						set_client_max_body_size(std::string arg);
		std::string					get_host(void) const;
		unsigned short				get_port(void) const;
		std::vector<std::string>	get_server_name(void) const;
		err_page_t					get_error_page(void) const;
		size_t						get_client_max_body_size(void) const;
		Location					location;
		void 						startListening(void);
		void 						stopListening(void);
		int							getListenFd();

};
std::ostream &	operator<<(std::ostream & os, const Server & server);
std::string		extract_parameters(const std::string & name, const std::string & directive);

#endif
