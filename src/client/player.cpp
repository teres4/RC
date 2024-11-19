#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.hpp"


// extern bool is_exiting; // flag to indicate whether the application is exiting


int main(int argc, char *argv[])
{
    setup_signal_handlers();  // change the signal handlers to our own
    Client client(argc, argv); // parse command-line arguments

    return 0;
}


Client::Client(int argc, char *argv[]) {
    if (argc == 3){ // check if -n or -p
        if (strcmp(argv[1], "-n") == 0) 
            host = argv[2];

        else if (strcmp(argv[1], "-p") == 0) 
            port = argv[2];
    }

    if (argc == 5){
        host = argv[2];
        port = argv[4];
    }

    validate_port(port);
}

