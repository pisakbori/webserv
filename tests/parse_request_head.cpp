#include "tests.hpp"

Request *parseRequest(std::string filename)
{
	std::stringstream buffer;

	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Error: Could not open the file: " + filename);
	}
	Request *req = new Request();
	buffer << file.rdbuf();
	std::string str = buffer.str();
	req->append(str);
	req->parseRequest();
	file.close();
	return req;
}

void valid_request()
{
	Request *req = parseRequest("./test_input/GET.txt");
	assert(req->getMethod() == "GET");
	assert(req->getUri() == "/index.html?user=123");
	assert(req->getProtocol() == "HTTP/1.1");
	assert(req->getHeader().find("HOST")->second == "example.com");
	assert(req->getHeader().find("USER-AGENT")->second == "curl/7.68.0");
	assert(req->getHeader().find("TRY")->second == "");
	delete req;
}

void bad_request()
{
	try
	{
		parseRequest("./test_input/control_characters.txt");
	}
	catch (HttpError &e)
	{
		assert(e.code() == 400);
	}
}

void test_parse_request_header(void)
{
	valid_request();
	bad_request();
}
