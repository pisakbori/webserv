#include "Webserv.hpp"
#include <csignal>

Webserv webserv;

void stop()
{
	std::cout << "stopping" << std::endl;
	webserv.stop();
}

void handle_sigint(int signal)
{
	(void)signal;
	webserv.stop();
	std::exit(0);
}

int main()
{
	std::signal(SIGINT, handle_sigint);
	webserv.configure("./test_input/1server.conf");
	webserv.run();
	return 0;
}
