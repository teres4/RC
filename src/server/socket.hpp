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
#include <stdexcept>
#include <fcntl.h>

#include "common/constants.hpp"
#include "common/utils.hpp"

class TcpServer
{
public:
    int _fd;                // The file descriptor of the socket
    struct addrinfo _hints; // The address flags
    struct addrinfo *_res;  // The address info

public:
    TcpServer(std::string gsport);
    ~TcpServer();
    void closeServer();

    std::string getClientIP();
    std::string getClientPort();
};

class UdpServer
{
private:
    int _fd;                  // The file descriptor of the socket
    struct addrinfo _hints;   // The address flags
    struct addrinfo *_res;    // The address info
    struct sockaddr_in _addr; // The address

public:
    UdpServer(std::string gsport);
    ~UdpServer();
    void send(std::string &message);
    std::string receive();
    void closeServer();

    std::string getClientIP();
    std::string getClientPort();
};

/**
 * @class SocketException
 * @brief Represents an exception that is thrown when a network error occurs.
 */
class SocketException : public std::runtime_error
{
public:
    /**
     * @brief Constructs a SocketException object.
     */
    SocketException() : std::runtime_error("A network error has occurred.") {};

    /**
     * @brief Destructor that shuts down the program.
     */
    ~SocketException() noexcept
    {
        exit(EXIT_FAILURE);
    }
};

#endif