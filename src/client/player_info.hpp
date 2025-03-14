#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

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

#include "common/constants.hpp"


/**
 * @class UDPInfo
 * @brief Represents a UDP client that can send and receive data over the
 * network.
 */
class UDPInfo {
  private:
    int _fd;                   // The file descriptor of the socket
    struct addrinfo _hints;    // The address flags
    struct addrinfo *_res;     // The address info
    struct sockaddr_in _addr;  // The address

  public:
    /**
     * @brief Constructs a UdpClient object with the specified hostname and
     * port.
     * @param hostname The hostname or IP address of the server.
     * @param port The port number to connect to.
     */
    UDPInfo(std::string hostname, std::string port);

    /**
     * @brief Destroys the UdpClient object and closes the socket.
     */
    ~UDPInfo();

    /**
     * @brief Sends the specified message to the server.
     * @param message The message to send.
     */
    void send(std::string &message);

    /**
     * @brief Receives a message from the server.
     * @return The received message as a string.
     */
    std::string receive();

};


/**
 * @class TcpClient
 * @brief Represents a TCP client that can send and receive data over the
 * network.
 */
class TCPInfo {
  private:
    int _fd;                 // The file descriptor of the socket
    struct addrinfo _hints;  // The address flags
    struct addrinfo *_res;   // The address info

  public:
    /**
     * @brief Constructs a TcpClient object with the specified hostname and
     * port.
     * @param hostname The hostname or IP address of the server.
     * @param port The port number to connect to.
     */
    TCPInfo(std::string hostname, std::string port);

    /**
     * @brief Destroys the TcpClient object and closes the socket.
     */
    ~TCPInfo();

    /**
     * @brief Sends the specified message to the server.
     * @param message The message to send.
     */
    void send(std::string &message);

    /**
     * @brief Receives a message from the server.
     * @return The received message as a string.
     */
    std::string receive();

    void closeTcpSocket();

};




/**
 * @class SocketException
 * @brief Represents an exception that is thrown when a network error occurs.
 */
class SocketException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a SocketException object.
     */
    SocketException() : std::runtime_error("A network error has occurred."){};
};


/**
 * @class TimeoutException
 * @brief Represents an exception that is thrown when the server has timed out.
 */
class TimeoutException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a TimeoutException object.
     */
    TimeoutException() : std::runtime_error("Server has timed out."){};
};

#endif