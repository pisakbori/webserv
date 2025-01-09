#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>
#include <Colors.h>

#include <unistd.h>

#define READ_BUFFER_SIZE 8192  // 8 KB buffer
#define WRITE_BUFFER_SIZE 8192 // 8 KB buffer
#define MAX_QUEUED 1000
#define CLIENT_HEADER_TIMEOUT 60  //  from established connection until finish sending header
#define CLIENT_BODY_TIMEOUT 60000 //  between successive read operations TODO:
#define KEEPALIVE_TIMEOUT 75      //   How long after sending resonse should we wait before closing connection.

#endif