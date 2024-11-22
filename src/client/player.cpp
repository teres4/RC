#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.hpp"
#include "commands.hpp"

// extern bool is_exiting; // flag to indicate whether the application is exiting

int main(int argc, char *argv[])
{
    setup_signal_handlers();   // change the signal handlers to our own
    Client client(argc, argv); // parse command-line arguments

    CommandManager commandManager; // create a new command manager

    addAllComands(commandManager);

    // player info`
    // setup sockets

    commandManager.waitForCommand();

    setUpSockets(client);

    // TODO: open sockets send t o command manager etc

    return 0;
}

Client::Client(int argc, char *argv[])
{
    if (argc == 3)
    { // check if -n or -p
        if (strcmp(argv[1], "-n") == 0)
            gsip = argv[2];

        else if (strcmp(argv[1], "-p") == 0)
            gsport = argv[2];
    }

    if (argc == 5)
    {
        gsip = argv[2];
        gsport = argv[4];
    }

    validate_port(gsport);
    validate_ip(gsip);
}

void addAllComands(CommandManager &manager)
{
    manager.addCommand(std::make_shared<StartCommand>());
    manager.addCommand(std::make_shared<TryCommand>());
    manager.addCommand(std::make_shared<QuitCommand>());
    manager.addCommand(std::make_shared<ShowTrialsCommand>());
    manager.addCommand(std::make_shared<ScoreboardCommand>());
    manager.addCommand(std::make_shared<ExitCommand>());
    manager.addCommand(std::make_shared<DebugCommand>());
}

void setUpSockets(Client &client)
{

    // create a socket
    client.udpSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (client.udpSockFD < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    // create a socket
    client.tcpSockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (client.tcpSockFD < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    // get server address
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int errcode = getaddrinfo(client.gsip.c_str(), client.gsport.c_str(), &hints, &client.serverUdpAddr);
    if (errcode != 0) /*error */
    {
        std::cerr << "Error getting address info" << std::endl;
        exit(1);
    }

    errcode = getaddrinfo(client.gsip.c_str(), client.gsport.c_str(), &hints, &client.serverTcpAddr);
    if (errcode != 0) /*error */
    {
        std::cerr << "Error getting address info" << std::endl;
        exit(1);
    }
}