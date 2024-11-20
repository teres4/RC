#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
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

    CommandManager commandManager;    // create a new command manager

    addAllComands(commandManager);

    // player info
    // setup sockets

    commandManager.waitForCommand();

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