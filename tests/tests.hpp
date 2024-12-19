#ifndef TESTS_HPP
#define TESTS_HPP

#include <cassert>
#include <iostream>
#include "../http_core_module/Server.hpp"
#include "../response_request/Validate.hpp"
#include "../response_request/HttpError.hpp"
#include "../response_request/Request.hpp"
#include "../response_request/Response.hpp"

void test_parse_listen(void);
void test_set_server_name(void);
void test_set_error_page(void);
void test_set_client_max_body_size(void);

void test_allow(void);
void test_redirect(void);
void test_root(void);
void test_autoindex(void);
void test_index(void);

void test_parse_request_header(void);
#endif
