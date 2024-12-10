#include "player_info.hpp"

UDPInfo::UDPInfo(std::string gsip, std::string gsport)
{
    _fd = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket

    if (_fd == -1)
    {
        throw SocketException();
    }

    // Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;

    int errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &_hints, &_res);

    if (errcode != 0)
    {
        throw SocketException();
    }

    // Set the timeout of the socket
    struct timeval timeout;
    timeout.tv_sec = SOCKETS_UDP_TIMEOUT;
    timeout.tv_usec = 0;

    // Set the timeout
    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw SocketException();
    }
}

UDPInfo::~UDPInfo()
{
    freeaddrinfo(_res); // Free the address info
    close(_fd);         // Close the socket
}

void UDPInfo::send(std::string &message)
{
    size_t n = message.length();

    // Send the message
    if (sendto(_fd, message.c_str(), n, 0, _res->ai_addr, _res->ai_addrlen) <= 0)
    {
        throw SocketException();
    }
}


std::string UDPInfo::receive()
{
    char buffer[BUFFER_SIZE + 1];
    socklen_t addrlen = sizeof(_addr); // Get the size of the address

    ssize_t n = recvfrom(_fd, buffer, BUFFER_SIZE + 1, 0, (struct sockaddr *)&_addr, &addrlen); // Receive the message

    if (n <= -1)
    {
        throw TimeoutException();
    }
    if (n > BUFFER_SIZE)
    {
        throw SocketException();
    }

    buffer[n] = '\n'; // Null-terminate the buffer to safely convert to string

    return std::string(buffer, (size_t)n);
}


TCPInfo::TCPInfo(std::string gsip, std::string gsport)
{
    _fd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
    if (_fd == -1)
    {
        throw SocketException();
    }

    struct timeval read_timeout;
    read_timeout.tv_sec = TCP_READ_TIMEOUT;
    read_timeout.tv_usec = 0;
    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) < 0) {
        throw SocketException();
    }
    struct timeval write_timeout;
    write_timeout.tv_sec = TCP_WRITE_TIMEOUT;
    write_timeout.tv_usec = 0;
    if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &write_timeout, sizeof(write_timeout)) < 0) {
        throw SocketException();
    }

    // Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;

    int errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &_hints, &_res);

    if (errcode != 0)
    {
        throw SocketException();
    }

    errcode = connect(_fd, _res->ai_addr, _res->ai_addrlen); // Connect to the server

    if (errcode == -1)
    {
        throw TimeoutException();
    }
}

TCPInfo::~TCPInfo()
{
    freeaddrinfo(_res); // Free the address info
    close(_fd);         // Close the socket
}

void TCPInfo::send(std::string &message)
{
    size_t message_size = message.size();
    size_t total_sent = 0;

    while (total_sent < message_size)
    {
        size_t bytes_to_send = std::min(message_size - total_sent, static_cast<size_t>(BUFFER_SIZE));
        ssize_t bytes_sent = write(_fd, message.data() + total_sent, bytes_to_send);

        if (bytes_sent == -1)
        {
            throw SocketException();
        }
        total_sent += (size_t)bytes_sent;
    }
}

std::string TCPInfo::receive()
{
    char buffer[BUFFER_SIZE];
    std::string message;

    ssize_t n = read(_fd, buffer, BUFFER_SIZE); // Read the message

    if (n == -1)
        throw SocketException();

    while (n != 0)
    {                                      // While there is content to be read
        message.append(buffer, (size_t)n); // Append the data to the string

        n = read(_fd, buffer, BUFFER_SIZE);

        if (n == -1)
        {
            throw SocketException();
        }
    }

    return message;
}


void TCPInfo::closeTcpSocket() {
    // freeaddrinfo(_res);
    if (close(_fd) != 0) {
        if (errno == EBADF)   // was already closed
            return;

        throw SocketException();
  }
}
