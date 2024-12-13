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
    bool _verbose = false;

public:
    GamedataManager _DB;
    Server(int argc, char **argv);
    void initializeServers();

    bool isverbose();

    std::string getPort();
};

#endif