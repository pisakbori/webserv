#include "tests.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "CommonIncludes.hpp"
#include "RunningServer.hpp"
#include <sys/event.h>
#include <Kernel/sys/event.h>

void parse_config_1_connection_accept_1_req(void)
{
    std::ifstream infile("./test_input/1server.conf");
    std::string line;
    Server serv;
    while (std::getline(infile, line))
    {
        if (line.find("server") == 0)
            serv.populate_server(infile);
    }
    // RunningServer rs = RunningServer(serv);
    RunningServer rs(serv);

    int
        kq,
        new_events;
    struct kevent change_event[1000],
        event[1000];

    kq = kqueue();

    // Create event 'filter', these are the events we want to monitor.
    // Here we want to monitor: rs.getListenFd(), for the events: EVFILT_READ
    // (when there is data to be read on the socket), and perform the following
    // actions on this kevent: EV_ADD and EV_ENABLE (add the event to the kqueue
    // and enable it).
    EV_SET(change_event, rs.getListenFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    // Register kevent with the kqueue.
    if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
    {
        throw std::runtime_error("kevent register with kqueue fail");
        exit(1);
    }

    std::map<int, Connection> connections;
    for (;;)
    {
        // Check for new events, but do not register new events with
        // the kqueue.
        new_events = kevent(kq, NULL, 0, event, 1, NULL);
        if (new_events == -1)
        {
            throw std::runtime_error("kevent new event");
            exit(1);
        }

        for (int i = 0; new_events > i; i++)
        {
            int event_fd = event[i].ident;
            // When the client disconnects an EOF is sent. By closing the file
            // descriptor the event is automatically removed from the kqueue.
            if (event[i].flags & EV_EOF)
            {
                connections[event_fd].process();
                std::cout << "Client has disconnected" << std::endl;
                connections.erase(event_fd);
                close(event_fd);
            }
            // a new client wants to connect to our socket.
            else if (event_fd == rs.getListenFd())
            {
                Connection c = Connection(&rs);
                connections[c.getFd()] = c;
                std::cout << "insert " << c.getFd() << std::endl;
                // Put this new socket connection also as a 'filter' event
                // to watch in kqueue, so we can now watch for events on this
                // new socket.
                EV_SET(change_event, c.getFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
                {
                    throw std::runtime_error("kevent error, fail to watch new kevent for fd ");
                }
            }

            else if (event[i].filter & EVFILT_READ)
            {
                char buf[READ_BUFFER_SIZE];
                size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
                std::cout << "read " << bytes_read << " bytes from " << event_fd << std::endl;
                std::string str(buf, bytes_read);
                auto found = connections.find(event_fd);
                if (found == connections.end())
                {
                    std::cerr << "Error: connection not in map\n";
                }
                else
                {
                    found->second.append(str);
                }
            }
        }
    }
}