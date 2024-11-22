#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "common/constants.hpp"
#include "common/utils.hpp"
#include "commands.hpp"

class Client
{
  public:
    // char* program_path;
    std::string gsip = DEFAULT_HOSTNAME;
    std::string gsport = DEFAULT_PORT;
    // bool help = false;

    // // the sockets
    // int udpSockFD = -1;
    // int tcpSockFD = -1;
    // struct addrinfo *serverUdpAddr = NULL;
    // struct addrinfo *serverTcpAddr = NULL;
    

    Client(int argc, char *argv[]);

    // void printHelp(std::ostream& stream);
};


void addAllCommands(CommandManager &manager);

// void setUpSockets(Client &client);

#endif