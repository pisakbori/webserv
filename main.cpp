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

void sigpipe_handler(int sig)
{
	(void)sig;
	// std::cerr << "SIGPIPE received " << sig << std::endl;
}

int main()
{
	std::signal(SIGINT, handle_sigint);
	signal(SIGPIPE, sigpipe_handler);
	webserv.configure("./test_input/aws.conf");
	// webserv.configure("./test_input/1server_indextest.conf");
	webserv.run();
	return 0;
}
