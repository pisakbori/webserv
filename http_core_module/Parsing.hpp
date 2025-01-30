#ifndef PARSING_HPP
# define PARSING_HPP

# include "Server.hpp"
# include <string>
# include <iostream>

class Server;

void trim(std::string& str, const std::string& whitespace = " \t");
std::string		extract_parameters(
	const std::string & name,
	std::string & directive
	);
void serverBlockValid(std::string & line);
void parse_config(std::string file_name, std::vector<Server> &servers);
void locationBlockValid(std::string &line);

#endif
