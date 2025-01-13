#include "http_core_module/Server.hpp"
#include "tests/tests.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

void	parse_config(std::string file_name, std::vector<Server> & servers)
{
	std::ifstream	infile(file_name);
	std::string		line;

	while (std::getline(infile, line))
	{
		Server	serv;
		if (line.find("server {") != std::string::npos)
			serv.populate_server(infile);
		servers.push_back(serv);
	}
}

int	main(int argc, char *argv[])
{
	std::vector<Server>	servers;
	if (argc != 2)
		return 1;
	parse_config(argv[1], servers);
	for (size_t i = 0; i < servers.size(); ++i)
		std::cout << servers.at(i) << std::endl;
	// test_parse_request_header();
	// test_server_parsing();
	return 0;
}
