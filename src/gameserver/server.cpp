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
#include "commands.cpp"

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    Server server(argc, argv);

    // open conections
    server.initializeServers();

    // CommandManager commandManager; // create a new command manager
    // commandManager.addAllCommands();

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
    validate_port(_gsport);
}

// setup sockets
void Server::initializeServers()
{
    try
    {
        UdpServer udpServer(_gsport);
        TcpServer tcpServer(_gsport);

        int pid = fork();
        if (pid > 0) // parent process
        {
            // udp
            tcpServer.closeServer();
        }
        else if (pid == 0) // child process
        {
            udpServer.closeServer();
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

void UDPServer(UdpServer udpServer, CommandManager commandManager, Server &server)
{
    bool verbose = server.isverbose();
    while (true)
    {
        std::string message = udpServer.receive();
        std::string response = commandManager.handleCommand(message, server);
        if (verbose)
        {
            std::cout << udpServer.ClientIP() << ":" << udpServer.ClientPort() << std::endl;
        }
    }
}