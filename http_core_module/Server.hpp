#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include "Listen.hpp"
#include "Location.hpp"
#include "CommonIncludes.hpp"
#include "HttpError.hpp"

/*
 * Server class stores information regarding server directive in the .conf
 * It's inspired by nginx directives:
 * https://nginx.org/en/docs/http/ngx_http_core_module.html
 * #TODO
 * - [ ] Check if port, host, error_code is in proper before assignment
 * - [ ] Server can have multiple locations
 */

typedef struct err_page_s
{
	std::vector<int> code;
	int overwrite;
	std::string uri;
} err_page_t;

class Server
{
	private:
		Listen						listen;
		std::vector<std::string>	server_name;
		err_page_t					error_page;
		long long					client_max_body_size;
		struct sockaddr_in			_serverAddr;

	// Setters
		void						set_server(std::string directive);
		void						parse_listen(std::string arg);
		void						set_server_name(std::string arg);
		void						set_error_page(std::string arg);
		void						set_client_max_body_size(std::string arg);

	public:
	// Constructor
		Server();

	// Copy constructor
		Server(Server const & src);

	// Destructor
		~Server();

	// Overloads
		Server &	operator=(Server const & rhs);

	// Member functions
		void						populate_server(std::ifstream & infile);

	// Getters
		Listen &					get_listen(void);
		const Listen &				get_listen(void) const;
		std::vector<std::string>	get_server_name(void) const;
		err_page_t					get_error_page(void) const;
		long long					get_client_max_body_size(void) const;
		std::vector<Location>		location;
		void						startListening(void);
		void						stopListening(void);
		int							getListenFd() const;
		const Location &			get_location(std::string uri) const;
};
std::ostream &	operator<<(std::ostream & os, const Server & server);
std::string		extract_parameters(
	const std::string & name,
	const std::string & directive
	);

#endif
