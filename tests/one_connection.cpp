#include "tests.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "CommonIncludes.hpp"

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

    // create a socket - Get the file descriptor!
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    // bind to an address -What port am I on?
    struct sockaddr_in serv_addr, cli_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv.get_host().c_str());
    serv_addr.sin_port = htons(serv.get_port());
    if (bind(fd, (struct sockaddr *)(&serv_addr), sizeof(serv_addr)) == -1)
        throw std::runtime_error("Could not bind socket to port " + std::to_string(serv.get_port()));
    // listen on a port, and wait for a connection to be established.
    listen(fd, MAX_QUEUED);
    std::cout << "server: listening on port " << serv.get_port() << std::endl;
    // accept the connection from a client.
    socklen_t cli_len = sizeof(cli_addr);
    send(message, serv_addr);
    int newsockfd = accept(fd, (struct sockaddr *)(&cli_addr), &cli_len);
    if (newsockfd < 0)
        throw std::runtime_error("ERROR on accept");
    int flags = fcntl(newsockfd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("ERROR getting socket flags");
    if (fcntl(newsockfd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("ERROR setting socket to non-blocking");

    std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << std::endl;

    char buffer[READ_BUFFER_SIZE];
    int n = read(newsockfd, buffer, READ_BUFFER_SIZE);
    if (n < 0)
        throw std::runtime_error("ERROR reading from socket");
    std::cout << "message: " << buffer << std::endl;
    assert(std::string(buffer) == message);
    close(newsockfd);
    close(fd);
}