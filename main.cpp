#include "http_core_module/Server.hpp"
#include "tests/tests.hpp"
#include <string>
#include <sstream>
#include <fstream>

void	parse_config(std::string file_name)
{
	std::ifstream	infile(file_name);
	std::string		line;
	Server			serv;

	while (std::getline(infile, line))
	{
		if (line.find("server") == 0)
			serv.populate_server(infile);
	}
	std::cout << serv << std::endl;
}

int	main(int argc, char *argv[])
{
	// test_parse_listen();
	// test_set_server_name();
	// test_set_client_max_body_size();
	// test_set_error_page();

	// test_allow();
	// test_redirect();
	// test_root();
	// test_autoindex();
	// test_index();
	if (argc != 2)
		return 1;
	parse_config(argv[1]);

	return 0;
}
