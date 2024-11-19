
#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <cstddef>

class Player_Info
{
public:
    int plid; // The username of the user
    bool hasActiveGame = false;
    int num_trials = 0;
    // a map for the players' attempts?

    // the sockets
    int udpSockFD = -1;
    int tcpSockFD = -1;
    struct addrinfo *serverUdpAddr = NULL;
    struct addrinfo *serverTcpAddr = NULL;

    Player_Info(int plid);
};

#endif