#include "Connection.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <Kernel/sys/event.h>

void readRequest(int event_fd, std::map<int, Connection> &connections)
{
	char buf[READ_BUFFER_SIZE];
	size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
	std::cout << "read " << bytes_read << " bytes from " << event_fd << std::endl;
	std::string str(buf, bytes_read);
	auto found = connections.find(event_fd);
	if (found == connections.end())
		std::cerr << "Error: connection not in map\n";
	else
		found->second.append(str);
}

void processRequest(int event_fd, std::map<int, Connection> &connections)
{
	connections[event_fd].process();
	std::cout << "Client has disconnected" << std::endl;
	connections.erase(event_fd);
	close(event_fd);
}

int main(int argc, char *argv[])
{
	(void)argv;
	(void)argc;
	std::ifstream infile("./test_input/1server.conf");
	std::string line;
	Server serv;
	struct kevent change_event[1000], event[1000];
	std::map<int, Connection> connections;

	while (std::getline(infile, line))
	{
		if (line.find("server") == 0)
			serv.populate_server(infile);
	}
	RunningServer rs(serv);

	int kq = kqueue();
	EV_SET(change_event, rs.getListenFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
	{
		throw std::runtime_error("kevent register with kqueue fail");
		exit(1);
	}

	for (;;)
	{
		int new_events = kevent(kq, NULL, 0, event, 1, NULL);
		if (new_events == -1)
			throw std::runtime_error("kevent new event");

		for (int i = 0; new_events > i; i++)
		{
			int event_fd = event[i].ident;
			if (event[i].flags & EV_EOF)
				processRequest(event_fd, connections);
			else if (event_fd == rs.getListenFd())
			{
				Connection c = Connection(&rs);
				connections[c.getFd()] = c;
				EV_SET(change_event, c.getFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
					throw std::runtime_error("kevent error, failed to watch new connection");
			}
			else if (event[i].filter & EVFILT_READ)
				readRequest(event_fd, connections);
		}
	}
	return 0;
}
