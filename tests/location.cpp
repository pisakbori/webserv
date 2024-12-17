#include "tests.hpp"

void	test_allow(void)
{
	std::string	inputs[] = {
		"allow HEAD GET POST;",
		"allow POST;",
		"allow ;"
	};
	std::vector<std::string> outputs[3];
	outputs[0].push_back("HEAD");
	outputs[0].push_back("GET");
	outputs[0].push_back("POST");
	outputs[1].push_back("POST");
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.get_location().set_allow(inputs[i]);
		std::vector<std::string>path(serv.get_location().get_allow());
		bool test = serv.get_location().get_allow() == outputs[i];
		if (!test) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
			for (std::vector<std::string>::const_iterator i = path.begin(); i != path.end(); ++i)
				std::cout << *i << ' ';
			std::cout << std::endl;
		}
	}
}

void	test_redirect(void)
{
	std::string	inputs[] = {
		"redirect 301 some_url;",
		"redirect some_url;",
		"redirect 502;",
	};
	std::pair<int, std::string> outputs[3];
	outputs[0].first = 301;
	outputs[0].second = "some_url";
	outputs[1].first = 0;
	outputs[1].second = "some_url";
	outputs[2].first = 502;
	outputs[2].second = "";
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.get_location().set_redirect(inputs[i]);
		bool test = serv.get_location().get_redirect() == outputs[i];
		if (!test) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}

void	test_root(void)
{
	std::string	inputs[] = {
		"root some_url;",
		"root /data/w3;"
	};
	std::string	outputs[] = {
		"some_url",
		"/data/w3"
	};
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.get_location().set_root(inputs[i]);
		bool test = serv.get_location().get_root() == outputs[i];
		if (!test) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}

void	test_autoindex(void)
{
	std::string	inputs[] = {
		"autoindex on;",
		"autoindex off;"
	};
	bool	outputs[] = {
		true,
		false
	};
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.get_location().set_autoindex(inputs[i]);
		bool test = serv.get_location().get_autoindex() == outputs[i];
		if (!test) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}

void	test_index(void)
{
	std::string	inputs[] = {
		"index index.$geo.html index.html;",
		"index index.html index.htm index.php;",
		"index index.html;"
	};
	std::vector<std::string> outputs[3];
	outputs[0].push_back("index.$geo.html");
	outputs[0].push_back("index.html");
	outputs[1].push_back("index.html");
	outputs[1].push_back("index.htm");
	outputs[1].push_back("index.php");
	outputs[2].push_back("index.html");
	size_t test_count = sizeof(inputs) / sizeof(inputs[0]);
	for (size_t i = 0; i < test_count; ++i)
	{
		Server serv;
		serv.get_location().set_index(inputs[i]);
		bool test = serv.get_location().get_index() == outputs[i];
		if (!test) {
			std::cerr << "Test " << i + 1 << " failed for input: \"" << inputs[i] << "\"\n";
		}
	}
}
