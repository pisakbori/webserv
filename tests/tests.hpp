#ifndef TESTS_HPP
# define TESTS_HPP

# include <cassert>
# include <iostream>
# include "../http_core_module/Server.hpp"

void	test_parse_listen(void);
void	test_set_server_name(void);
void	test_set_error_page(void);
void	test_set_client_max_body_size(void);

void	test_allow(void);
void	test_redirect(void);
void	test_root(void);
void	test_autoindex(void);
void	test_index(void);

#endif
