#include "http_core_module/Server.hpp"
#include "tests/tests.hpp"
#include <string>
#include <sstream>
#include <fstream>

void parse_config(std::string file_name)
{
	std::ifstream infile(file_name);
	std::string line;
	Server serv;

	while (std::getline(infile, line))
	{
		if (line.find("server") == 0)
			serv.populate_server(infile);
	}
	std::cout << serv << std::endl;
}

// int	main(int argc, char *argv[])
// {
// 	if (argc != 2)
// 		return 1;
// 	parse_config(argv[1]);

// 	test_parse_request_header();
// 	test_server_parsing();
// 	return 0;
// }

int main(void)
{
	parse_config_1_connection_accept_1_req();
	return 0;
}
