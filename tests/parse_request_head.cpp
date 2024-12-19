#include "tests.hpp"

Request* parseRequest(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Error: Could not open the file: " + filename);
	}
	Request* req = new Request(file);
	file.close();
	return req;
}

void valid_request()
{
	Request* req = parseRequest("./test_input/GET.txt");
	assert(req->getMethod() == "GET");
	assert(req->getUri() == "/index.html?user=123");
	assert(req->getProtocol() == "HTTP/1.1");
	assert(req->getHeader().find("HOST")->second == "example.com");
	assert(req->getHeader().find("USER-AGENT")->second == "curl/7.68.0");
	assert(req->getHeader().find("TRY")->second == "");
}

void bad_request()
{
	try
	{
		Request* req = parseRequest("./test_input/control_characters.txt");
		(void) req;
	}
	catch (HttpError& e)
	{
		assert(e.code() == 400);
	}
}

void test_parse_request_header(void)
{
	valid_request();
	bad_request();
}
