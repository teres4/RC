#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "../common/constants.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    Server server(argc, argv);

    // open conections

    return 0;
}

Server::Server(int argc, char **argv)
{

    if (argc == 4)
    {
        if (strcmp(argv[1], "-p"))
        {
            _gsport = argv[2];
        }
        if (strcmp(argv[3], "-v"))
        {
            _verbose = true;
        }
        else
        {
            std::cout << "wrong args";
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-p"))
        {
            _gsport = argv[2];
        }
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-v"))
        {
            _verbose = true;
        }
    }
    else
        std::cout << "wrong args";
}

// setup sockets
void Server::InitializeServers()
{
}