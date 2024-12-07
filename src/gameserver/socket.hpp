#ifndef SOCKET_H
#define SOCKET_H

#include <sstream>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#include "../common/constants.hpp"

class TcpServer
{
private:
    int _fd;                  // The file descriptor of the socket
    struct addrinfo _hints;   // The address flags
    struct addrinfo *_res;    // The address info
    struct sockaddr_in _addr; // The address

public:
    TcpServer(int gsport);
};

class UdpServer
{
private:
    int _fd;                  // The file descriptor of the socket
    struct addrinfo _hints;   // The address flags
    struct addrinfo *_res;    // The address info
    struct sockaddr_in _addr; // The address

public:
    UdpServer(int gsport);
};

#endif