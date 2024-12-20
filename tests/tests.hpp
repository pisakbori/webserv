#ifndef TESTS_HPP
#define TESTS_HPP

#include <cassert>
#include <iostream>
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"

void test_parse_listen(void);
void test_set_server_name(void);
void test_set_error_page(void);
void test_set_client_max_body_size(void);

void test_allow(void);
void test_redirect(void);
void test_root(void);
void test_autoindex(void);
void test_index(void);

void test_server_parsing(void);
void test_parse_request_header(void);
void parse_config_1_connection_accept_1_req(void);
int send(std::string str, struct sockaddr_in serv_addr);

#endif
