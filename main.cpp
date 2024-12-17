#include "http_core_module/Server.hpp"
#include "tests/tests.hpp"

int	main( void )
{
	test_parse_listen();
	test_set_server_name();
	test_set_client_max_body_size();
	test_set_error_page();

	test_allow();
	test_redirect();
	test_root();
	test_autoindex();
	test_index();

	return 0;
}
