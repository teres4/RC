#ifndef SERVER_H
#define SERVER_H

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
#include "socket.hpp"

// #include "commands.hpp"
#include "database.hpp"

class Server
{
private:
    std::string _gsport = DEFAULT_PORT;
    // std::string _gsip;
    bool _verbose = false;

public:
    GamedataManager _DB = GamedataManager();
    Server(int argc, char **argv);
    // void initializeServers();

    // std::string getIPaddress();
    bool isverbose();

    std::string getPort();
};

#endif