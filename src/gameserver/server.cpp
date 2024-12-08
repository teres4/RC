#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdexcept>

#include "server.hpp"

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    Server server(argc, argv);

    // open conections
    server.InitializeServers();

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
            std::cout << "Wrong args/n Correct usage: [-p GSport] [-v]";
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
        std::cout << "Wrong args/n Correct usage: [-p GSport] [-v]";
}

// setup sockets
void Server::InitializeServers()
{
    try
    {
        UdpServer UdpServer(_gsport);
        TcpServer TcpServer(_gsport);

        int pid = fork();
        if (pid > 0) // parent process
        {
            // udp
            UdpServer.closeServer();
        }
        else if (pid == 0) // child process
        {
            TcpServer.closeServer();
        }
        else // fork failed
        {
            exit(1);
        }
    }
    catch (SocketException &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

bool Server::isverbose()
{
    return _verbose;
}