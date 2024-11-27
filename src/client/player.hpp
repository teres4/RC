#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
// #include "commands.hpp"

class Client
{
  public:
    std::string gsip = DEFAULT_HOSTNAME;
    std::string gsport = DEFAULT_PORT;
    // char* program_path;

    Client(int argc, char *argv[]);

    void processRequest(ProtocolCommunication &comm);


    // void printHelp(std::ostream& stream);
};




#endif