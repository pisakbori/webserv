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

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return 1;
	}

	std::signal(SIGINT, handle_sigint);
	signal(SIGPIPE, sigpipe_handler);

	try
	{
		webserv.configure(argv[1]);
		webserv.run();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
