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
#include "commands.hpp"

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server);

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server);

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    Server server(argc, argv);

    // open conections

    CommandManager commandManager; // create a new command manager
    commandManager.registerAllCommands();

    // server.initializeServers();
    try
    {
        UdpServer udpServer(server.getPort());
        TcpServer tcpServer(server.getPort());

        int pid = fork();

        if (pid < 0) // fork failed
            exit(1);

        else if (pid == 0) // child process
        {
            tcpServer.closeServer();
            // udp
            UDPServer(udpServer, commandManager, server); // start udp server
        }
        else // parent process
        {
            udpServer.closeServer();
            TCPServer(tcpServer, commandManager, server);
        }
    }
    catch (SocketException &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // while (!std::cin.eof() && !exiting){
    //     // commandManager.handleCommand();
    // }

    return 0;
}

Server::Server(int argc, char **argv)
{
    switch (argc)
    {
    case 1: // no arguments
        return;
    case 2: // with -v
        if (strcmp(argv[1], "-v") == 0)
            _verbose = true;
        else
            std::cout << "Wrong args\nCorrect usage: [-p GSport] [-v]\n";
        return;
    case 3: // with -p [GSport]
        if (strcmp(argv[1], "-p") == 0)
            _gsport = argv[2];
        else
            std::cout << "Wrong args\nCorrect usage: [-p GSport] [-v]\n";
        break;
    case 4: // with -p [GSport] -v
        if (strcmp(argv[1], "-p") == 0)
            _gsport = argv[2];

        if (strcmp(argv[3], "-v") == 0)
            _verbose = true;
        else
            std::cout << "Wrong args\nCorrect usage: [-p GSport] [-v]\n";
        break;
    default:
        std::cout << "Wrong args\nCorrect usage: [-p GSport] [-v]\n";
        return;
    }

    validate_port(_gsport);
    
}


bool Server::isverbose()
{
    return _verbose;
}

std::string Server::getPort()
{
    return _gsport;
}

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server)
{
    bool verbose = server.isverbose();

    while (true)
    {
        std::string message = udpServer.receive();
        std::cout << "in udpserver received: " << message;
        std::string response = manager.handleCommand(message, server);
        if (verbose)
        {
            std::cout << udpServer.getClientIP() << ":" << udpServer.getClientPort() << std::endl;
        }
        std::cout << "udp response: " << response;
        udpServer.send(response);
    }
}

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server)
{
    bool verbose = server.isverbose();
    tcpServer.accept();

    while (true)
    {
        std::string message = tcpServer.receive();
        std::cout << "in tcpserver received: " << message;
        std::string response = manager.handleCommand(message, server);
        if (verbose)
        {
            std::cout << tcpServer.getClientIP() << ":" << tcpServer.getClientPort() << std::endl;
        }
        std::cout << "tcp response: " << response;
        tcpServer.send(response);
    }
}