#include "player_info.hpp"


Player_Info::Player_Info(std::string &hostname, std::string &port) {
  this->setupSockets();
  this->setupServerAddr(hostname, port);
}


void Player_Info::setupSockets() {
    // create a udp socket
    udpSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSockFD < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    // create a tcp socket
    tcpSockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSockFD < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

   
}

void Player_Info::setupServerAddr(std::string &gsip, std::string &gsport) {
    // get server address
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &hints, &serverUdpAddr);
    if (errcode != 0) /*error */
    {
        std::cerr << "Error getting address info" << std::endl;
        exit(1);
    }

    errcode = getaddrinfo(gsip.c_str(), gsport.c_str(), &hints, &serverTcpAddr);
    if (errcode != 0) /*error */
    {
        std::cerr << "Error getting address info" << std::endl;
        exit(1);
    }
}