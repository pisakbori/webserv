#include "Server.hpp"
#include "tests.hpp"

int	main( void )
{
	test_parse_listen();
	test_set_server_name();
	test_set_client_max_body_size();
}
