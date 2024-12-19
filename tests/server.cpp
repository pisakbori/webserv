#include "tests.hpp"

void	test_parse_listen(void)
{
	std::string inputs[] = {
		"127.0.0.1:8000",
		"127.0.0.1",
		"8000",
		"*:8000",
		"localhost:8000",
		"[::]:8000",
		"[::1]"
	};

	unsigned short expected_ports[] = {
		8000,
		80,
		8000,
		8000,
		8000,
		8000,
		80
	};

	std::string expected_hosts[] = {
		"127.0.0.1",
		"127.0.0.1",
		"0.0.0.0",
		"*",
		"localhost",
		"[::]",
		"[::1]"
	};

	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i) {
		Server serv;
		serv.parse_listen(inputs[i]);
		bool host_match = serv.get_host() == expected_hosts[i];
		bool port_match = serv.get_port() == expected_ports[i];

		if (!host_match || !port_match) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
			if (!host_match) {
				std::cerr << "  Expected host: " << expected_hosts[i] 
							<< ", but got: " << serv.get_host() << "\n";
			}
			if (!port_match) {
				std::cerr << "  Expected port: " << expected_ports[i] 
							<< ", but got: " << serv.get_port() << "\n";
			}
		}
	}
}


void	test_set_server_name(void)
{
	std::string inputs[] = {
		"example.com www.example.com",
		"example.com *.example.com www.example.*",
		".example.com",
		"_"
	};
	std::vector<std::string> expected_names[4];
	expected_names[0].push_back("example.com");
	expected_names[0].push_back("www.example.com");
	expected_names[1].push_back("example.com");
	expected_names[1].push_back("*.example.com");
	expected_names[1].push_back("www.example.*");
	expected_names[2].push_back(".example.com");
	expected_names[3].push_back("_");

	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i) {
		Server serv;
		serv.set_server_name(inputs[i]);
		bool name_match = serv.get_server_name() == expected_names[i];
		if (!name_match) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}

void	test_set_error_page(void)
{
	std::string inputs[] = {
		"404             /404.html",
		"500 502 503 504 /50x.html",
		"404 =200 /empty.gif",
		"404 = /404.php",
		"403      http://example.com/forbidden.html",
		"404 =301 http://example.com/notfound.html"
	};
	std::vector<int> just_403;
	just_403.push_back(403);
	std::vector<int> just_404;
	just_404.push_back(404);
	std::vector<int> multiple_codes;
	multiple_codes.push_back(500);
	multiple_codes.push_back(502);
	multiple_codes.push_back(503);
	multiple_codes.push_back(504);
	err_page_t outputs[] = {
		{just_404, 200, "/404.html"},
		{multiple_codes, 200, "/50x.html"},
		{just_404, 200, "/empty.gif"},
		{just_404, 0, "/404.php"},
		{just_403, 200, "http://example.com/forbidden.html"},
		{just_404, 301, "http://example.com/notfound.html"}
	};
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.set_error_page(inputs[i]);
		bool codes = serv.get_error_page().code == outputs[i].code;
		bool overwrites = serv.get_error_page().overwrite == outputs[i].overwrite;
		bool uris = serv.get_error_page().uri == outputs[i].uri;
		if (!codes || !overwrites || !uris) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}

void	test_set_client_max_body_size(void)
{
	std::string inputs[] = {
		"2m",
		"1024",
		"14k",
	};
	size_t		outputs[] = {
		2 << 20,
		1024,
		14 << 10
	};
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.set_client_max_body_size(inputs[i]);
		bool size_match = serv.get_client_max_body_size() == outputs[i];
		if (!size_match) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
			if (!size_match) {
				std::cerr << "  Expected size: " << outputs[i] 
							<< ", but got: " << serv.get_client_max_body_size() << "\n";
			}
		}
	}
}

void	test_server_parsing(void)
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
}
