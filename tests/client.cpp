#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "CommonIncludes.hpp"

int send(std::string str, struct sockaddr_in serv_addr)
{
    char buffer[READ_BUFFER_SIZE];
    if (str.size() >= READ_BUFFER_SIZE)
        throw std::runtime_error("String is too large for the buffer");
    std::strncpy(buffer, str.c_str(), READ_BUFFER_SIZE);
    buffer[READ_BUFFER_SIZE - 1] = '\0';

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw std::runtime_error("Error opening socket");
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        throw std::runtime_error("ERROR connecting");
    int n = write(sockfd, buffer, READ_BUFFER_SIZE);
    if (n < 0)
        throw std::runtime_error("ERROR writing to socket");
    close(sockfd);
    return 0;
}