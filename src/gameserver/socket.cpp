#include "socket.hpp"

UdpServer::UdpServer(std::string gsport)
{
    int errcode;

    _fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_fd == -1) /*error*/
    {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;      // IPv4
    _hints.ai_socktype = SOCK_DGRAM; // UDP socket
    _hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, gsport.c_str(), &_hints, &_res);
    if (errcode != 0) /*error*/
    {
        throw SocketException();
    }

    errcode = bind(_fd, _res->ai_addr, _res->ai_addrlen);
    if (errcode == -1) /*error*/
    {
        throw SocketException();
    }
}

void UdpServer::send(std::string &message)
{
    size_t n = message.size();
    if (n <= 0)
    {
        throw SocketException();
    }
    // Send the message
    if (sendto(_fd, message.c_str(), (size_t)n, 0, _res->ai_addr, _res->ai_addrlen) <= 0)
    {
        throw SocketException();
    }
}

std::string UdpServer::receive()
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recvfrom(_fd, buffer, BUFFER_SIZE, 0, _res->ai_addr, &_res->ai_addrlen);
    if (bytes_received == -1)
    {
        throw SocketException();
    }
    return std::string(buffer, bytes_received);
}

UdpServer::~UdpServer()
{
    freeaddrinfo(_res); // Free the address info
    close(_fd);         // Close the socket
}

void UdpServer::closeServer()
{
    freeaddrinfo(_res);
    close(_fd);
}

TcpServer::TcpServer(std::string gsport)
{
    int errcode;

    _fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (_fd == -1)
    {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;       // IPv4
    _hints.ai_socktype = SOCK_STREAM; // TCP socket
    _hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, gsport.c_str(), &_hints, &_res);
    if ((errcode) != 0)
    {
        throw SocketException();
    }

    errcode = bind(_fd, _res->ai_addr, _res->ai_addrlen);
    if (errcode == -1)
    {
        throw SocketException();
    }

    if (listen(_fd, 5) == -1) /*error*/
    {
        throw SocketException();
    }
}

void TcpServer::send(std::string &message)
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

std::string TcpServer::receive(int fd)
{
    // between the initialization of TcpServer and receive(), there is an accept() call
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = read(fd, buffer, BUFFER_SIZE);
    if (bytes_received == -1)
    {
        throw SocketException();
    }
    return std::string(buffer, bytes_received);
}

int TcpServer::accept()
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_fd = ::accept(_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1)
    {
        throw SocketException();
    }
    return client_fd;
}

TcpServer::~TcpServer()
{
    freeaddrinfo(_res); // Free the address info
    close(_fd);         // Close the socket
}

void TcpServer::closeServer()
{
    freeaddrinfo(_res);
    close(_fd);
}

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