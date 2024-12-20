#include "tests.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "CommonIncludes.hpp"
#include "RunningServer.hpp"

void parse_config_1_connection_accept_1_req(void)
{
    std::ifstream infile("./test_input/1server.conf");
    std::string line;
    Server serv;
    std::string message = "Just a message";
    while (std::getline(infile, line))
    {
        if (line.find("server") == 0)
            serv.populate_server(infile);
    }
    RunningServer rs = RunningServer(serv);

    send(message, rs.getServerAddress());
    Connection c = Connection(&rs);
    char buffer[READ_BUFFER_SIZE];
    int n = read(c.getFd(), buffer, READ_BUFFER_SIZE);
    if (n < 0)
        throw std::runtime_error("ERROR reading from socket");
    std::cout << "message: " << buffer << std::endl;
    assert(std::string(buffer) == message);
}