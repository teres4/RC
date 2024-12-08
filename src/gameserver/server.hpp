#ifndef SERVER_H
#define SERVER_H

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
#include "socket.hpp"

class Server
{
private:
    std::string _gsport = DEFAULT_HOSTNAME;
    bool _verbose = false;

public:
    Server(int argc, char **argv);
    void initializeServers();

    bool isverbose();
};

#endif