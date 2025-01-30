#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sstream>
#include "Listen.hpp"
#include "Location.hpp"
#include "CommonIncludes.hpp"
#include "HttpError.hpp"
#include "Parsing.hpp"


typedef struct err_page_s
{
	std::vector<int> code;
	int overwrite;
	std::string uri;
} err_page_t;
class Location;
class Server
{
	private:
		Listen						listen;
		std::vector<std::string>	server_name;
		std::vector<err_page_t>		error_page;
		long long					client_max_body_size;
		struct sockaddr_in			_serverAddr;
		bool						listen_set;
		bool						client_max_body_size_set;

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
		void						populate_server(std::istringstream & infile);

	// Getters
		Listen &					get_listen(void);
		const Listen &				get_listen(void) const;
		std::vector<std::string>	get_server_name(void) const;
		std::vector<err_page_t>		get_error_page(void) const;
		err_page_t					get_error_page(const int & e_code) const;
		long long					get_client_max_body_size(void) const;
		std::vector<Location>		location;
		void						startListening(void);
		void						stopListening(void);
		int							getListenFd() const;
		const Location &			get_location(std::string uri) const;
};
std::ostream &	operator<<(std::ostream & os, const Server & server);

#endif
