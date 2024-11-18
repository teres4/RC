#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>
#include <iostream>

#include "../common/constants.hpp"
#include "../common/utils.hpp"


// configuration options for a client application.

class ClientConfig {
public:
    char *programPath;
    std::string host = DEFAULT_HOSTNAME;
    std::string port = DEFAULT_PORT;
    // bool help = false;

    /**
     * Constructs a new ClientConfig object.
     */
    ClientConfig(int argc, char *argv[]);


    // Prints the help menu.
    // void printHelp(std::ostream &stream);
  };



#endif


