/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:02:07 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/14 23:23:58 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

// # include <map>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>

/*
Server class stores information regarding server directive in the configuration
It's inspired by nginx directives: 
https://nginx.org/en/docs/http/ngx_http_core_module.html
#TODO
- [ ] (?) Inherit from the http directive
- [ ] Implement location member: vvv
https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
- [ ] Implement error_page member
- [ ] If cannot listen on default port 80 try 8080
*/

class Server
{

	private:

		std::string					host;
		unsigned short				*ports;
		std::string					*server_name;
		bool						is_default;
		// std::map<int, std::string>	error_page;
		size_t						client_max_body_size;
		// location

	public:

		Server();
		~Server();


};

#endif
