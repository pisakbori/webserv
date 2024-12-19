// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <cstring>
// #include <iostream>

// void	send_message( const char * message, const char * port )
// {
// 	int	sockfd, new_fd;
// 	struct addrinfo hints;
// 	struct addrinfo *servinfo;
// 	socklen_t addr_size;
// 	struct sockaddr_storage their_addr;

// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE;
// 	getaddrinfo(NULL, port, &hints, &servinfo);
// 	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
// 	bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
// 	listen(sockfd, 1);
// 	addr_size = sizeof their_addr;
//     new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
// 	send(new_fd, message, strlen(message), 0);
// 	freeaddrinfo(servinfo);
// }

// int	main( int argc, char *argv[] )
// {
// 	if (argc != 3)
// 	{
// 		std::cerr << "Usage: ./send 'message' port" << std::endl;
// 		return 1;
// 	}
// 	send_message(argv[1], argv[2]);
// 	return 0;
// }

// c++ -std=c++98 -Wall -Wextra -Werror main.cpp -o send
// nc localhost 'port'
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 19:36:32 by bpisak-l          #+#    #+#             */
/*   Updated: 2024/12/18 22:15:47 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validate.hpp"
#include "HttpError.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

int readFile(std::string filename)
{
	std::cout << filename << std::endl;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open the file: " << filename << std::endl;
		return 1;
	}
	// buffer << file.rdbuf();
	// std::string str = buffer.str();
	Request req = Request(file);
	file.close();
	return 0;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	try
	{
		// readFile("test-requests/control_characters.txt");
		readFile("test-requests/GET.txt");
		// readFile("test-requests/valid_nl.txt");
	}
	catch (HttpError &e)
	{
			// }
	// catch (std::exception &e)
	// {
	// 	throw HttpError("Bad Request: Invalid method", 400);
	// }
		std::clog << "Error: " << e.what() << std::endl;
		Response res = Response();
		std::clog << e.code() <<": " << res.statuses.find(e.code())->second <<std::endl;
	}
	return 0;
}
