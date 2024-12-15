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

#include "common/constants.hpp"
#include "common/utils.hpp"

class TcpServer
{
private:
    int _fd;                  // The file descriptor of the socket
    struct addrinfo _hints;   // The address flags
    struct addrinfo *_res;    // The address info
    int _clientfd;
    // struct sockaddr_in _addr; // The address

    bool _closed = false;  // Flag indicating whether the server has been closed

public:
    TcpServer(std::string gsport);
    ~TcpServer();
    void send(std::string &message);
    std::string receive();
    void closeServer();
    int accept();

    std::string getClientIP();
    std::string getClientPort();
    void setClientFd(int clientFd);

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
};

#endif