#include "player_info.hpp"


UDPInfo::UDPInfo(std::string gsip, std::string gsport) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);  // Create a UDP socket

    if (_fd == -1) {
        throw SocketException();
    }

    // Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;

    int errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &_hints, &_res); 

    if (errcode != 0) {
        throw SocketException();
    }

    // // Set the timeout of the socket
    // struct timeval timeout;
    // timeout.tv_sec = SOCKETS_UDP_TIMEOUT;
    // timeout.tv_usec = 0;

    // // Set the timeout
    // if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    //     throw SocketException();
    // }
}



UDPInfo::~UDPInfo() {
    freeaddrinfo(_res);  // Free the address info
    close(_fd);          // Close the socket
}
   

void UDPInfo::send(std::stringstream &message) {
    char buffer[BUFFER_SIZE];
    std::streamsize n = message.gcount();  // Get the size of the message

    message.read(buffer, n);  // Read the message

    if (n == -1) {
        throw TimeoutException();
    }
    // Send the message
    if (sendto(_fd, buffer, (size_t) n, 0, _res->ai_addr, _res->ai_addrlen) != n) {  
        throw SocketException();
    }
}

std::stringstream UDPInfo::receive() {
    char buffer[BUFFER_SIZE + 1];  
    socklen_t addrlen = sizeof(_addr);  // Get the size of the address
    ssize_t n = recvfrom(_fd, buffer, BUFFER_SIZE + 1, 0, (struct sockaddr *) &_addr, &addrlen);  // Receive the message


    std::cout << "in udp receive. received message: " << buffer;

    
    if (n == -1) {
        throw TimeoutException();
    }
    if (n > BUFFER_SIZE) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(buffer, (std::streamsize)n);  // Write the message to a stringstream



    return message;
}




TCPInfo::TCPInfo(std::string gsip, std::string gsport) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);  // Create a TCP socket
    if (_fd == -1) {
        throw SocketException();
    }

    // Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;

    int errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &_hints, &_res);  

    if (errcode != 0) {
        throw SocketException();
    }

    errcode = connect(_fd, _res->ai_addr, _res->ai_addrlen);  // Connect to the server

    if (errcode == -1) {
        throw TimeoutException();
    }
}


TCPInfo::~TCPInfo() {
    freeaddrinfo(_res);  // Free the address info
    close(_fd);          // Close the socket
}


void TCPInfo::send(std::stringstream &message) {
    ssize_t n = message.gcount();
    char buffer[BUFFER_SIZE];

    message.read(buffer, n);  // Read the message

    while (n != 0) {  // While there is content to be read, loops and writes to the socket
        if (write(_fd, buffer, (size_t)n) == -1) {
            throw SocketException();
        }
        message.read(buffer, BUFFER_SIZE);
        n = message.gcount();
    }
}


std::stringstream TCPInfo::receive() {
    char buffer[BUFFER_SIZE];
    std::stringstream message;

    ssize_t n = read(_fd, buffer, BUFFER_SIZE);  // Read the message

    if (n == -1) 
        throw SocketException();

    while (n != 0) {  // While there is content to be read
        message.write(buffer, n);

        n = read(_fd, buffer, BUFFER_SIZE);

        if (n == -1) {
            throw SocketException();
        }
    }

    return message;
}