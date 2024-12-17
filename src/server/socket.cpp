#include "socket.hpp"

UdpServer::UdpServer(std::string gsport)
{
    int errcode;

    _fd = socket(AF_INET, SOCK_DGRAM, 0); // create udp socket
    if (_fd == -1) {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;
    _hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, gsport.c_str(), &_hints, &_res);
    if (errcode != 0) {
        throw SocketException();
    }

    errcode = bind(_fd, _res->ai_addr, _res->ai_addrlen);
    if (errcode == -1) {
        throw SocketException();
    }
    std::cout << "UDP server initialized on port " << gsport << std::endl;
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
    return std::string(buffer, (size_t)bytes_received);
}


std::string UdpServer::getClientIP()
{
    struct sockaddr_in *addr = (struct sockaddr_in *)_res->ai_addr;
    return inet_ntoa(addr->sin_addr);
}


std::string UdpServer::getClientPort()
{
    struct sockaddr_in *addr = (struct sockaddr_in *)_res->ai_addr;
    return std::to_string(ntohs(addr->sin_port));
}


UdpServer::~UdpServer()
{
    freeaddrinfo(_res); // Free the address info
    close(_fd);         // Close the socket
}


void UdpServer::closeServer()
{
    if (close(_fd) != 0) {
        if (errno == EBADF) {  // was already closed
            return;
        }
        throw SocketException();
    }
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

    if (listen(_fd, 5) == -1) // error
    {
        throw SocketException();
    }
    std::cout << "TCP server initialized on port " << gsport << std::endl;
}


void TcpServer::send(std::string &message)
{
    size_t message_size = message.length();
    size_t total_sent = 0;

    while (total_sent < message_size)
    {
        ssize_t sent = write(_clientfd, message.data() + total_sent, message_size - total_sent);

        if (sent < 0) {
            throw SocketException();
        }
        total_sent += (size_t)sent;
    }
}

std::string TcpServer::receive()
{
    // between the initialization of TcpServer and receive(), there is an accept() call
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = read(_clientfd, buffer, BUFFER_SIZE);
    if (bytes_received == -1)
    {   
        throw SocketException();
    }
    
    return std::string(buffer, (size_t)bytes_received);
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


std::string TcpServer::getClientIP()
{
    struct sockaddr_in *addr = (struct sockaddr_in *)_res->ai_addr;
    return inet_ntoa(addr->sin_addr);
}


std::string TcpServer::getClientPort()
{
    struct sockaddr_in *addr = (struct sockaddr_in *)_res->ai_addr;
    return std::to_string(ntohs(addr->sin_port));
}


void TcpServer::setClientFd(int fd)
{
    _clientfd = fd;
}



TcpServer::~TcpServer()
{
    freeaddrinfo(_res); // Free the address info
    close(_clientfd);
    close(_fd);         // Close the socket
}


void TcpServer::closeServer() {
    if (close(_fd) != 0 || close(_clientfd)) {
        if (errno == EBADF)   // was already closed
            return;
        throw SocketException();
    }
}

