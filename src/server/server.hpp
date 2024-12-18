#ifndef SERVER_H
#define SERVER_H

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
#include "socket.hpp"

#include "database.hpp"

class Server
/**
 * @class Server
 * @brief Manages server operations and configurations.
 *
 * This class is responsible for handling server operations, like starting and managing the TCP UDP servers.
 *
 */
{
private:
    std::string _gsport = DEFAULT_PORT;
    bool _verbose = false;

public:
    GamedataManager _DB = GamedataManager();
    Server(int argc, char **argv);

    bool isverbose();

    std::string getPort();
};

#endif