#include "socket.hpp"

TcpServer::TcpServer(int gsport)
{
    int errcode;

    _fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (_fd == -1)
        exit(1); // error

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;       // IPv4
    _hints.ai_socktype = SOCK_STREAM; // TCP socket
    _hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, gsport, &_hints, &_res);
    if ((errcode) != 0) /*error*/
        exit(1);

    int n = bind(_fd, _res->ai_addr, _res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
    if (listen(_fd, 5) == -1) /*error*/
        exit(1);
}