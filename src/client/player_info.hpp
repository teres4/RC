#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <cstddef>
#include <iostream>

class Player_Info
{
    // the sockets
public:
    int udpSockFD = -1;
    int tcpSockFD = -1;
    struct addrinfo *serverUdpAddr = NULL;
    struct addrinfo *serverTcpAddr = NULL;

    void setupSockets();
    void setupServerAddr(std::string &hostname, std::string &port);

public:
    Player_Info(std::string &hostname, std::string &port);
    // ~PlayerState();

    // bool hasActiveGame();
    // bool hasGame();

    int plid; // The player id
    int max_playtime = 0;
    bool hasActiveGame = false;
    int num_trials = 0;
    // a map for the players' attempts?

    // Player_Info(int plid);
};

#endif