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
    
    // CommandManager commandManager; // create a new command manager
    // commandManager.addAllCommands();
    
    server.initializeServers();

    return 0;
}


Server::Server(int argc, char **argv)
{
    switch (argc){
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
            // UDPServer(udpServer, CommandManager(), *this);
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

// void UDPServer(UdpServer udpServer, CommandManager commandManager, Server &server)
// {
//     bool verbose = server.isverbose();
//     while (true)
//     {
//         std::string message = udpServer.receive();
//         std::string response = commandManager.handleCommand(message, server);
//         if (verbose)
//         {
//             std::cout << udpServer.ClientIP() << ":" << udpServer.ClientPort() << std::endl;
//         }
//         udpServer.send(response);
//     }
// }