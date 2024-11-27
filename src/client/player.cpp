#include <iostream>
#include "player.hpp"
#include "commands.hpp"

// extern bool is_exiting; // flag to indicate whether the application is exiting

int main(int argc, char *argv[])
{
    setup_signal_handlers();   // change the signal handlers to our own
    Client client(argc, argv); // parse command-line arguments

    CommandManager commandManager; // create a new command manager
    commandManager.addAllCommands();

    while (!std::cin.eof())
    {
        commandManager.waitForCommand(client);
    }

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

}

void Client::processRequest(ProtocolCommunication &comm) {
    std::stringstream reqMessage = comm.encodeRequest();
    std::stringstream resMessage;


    char buffer[BUFFER_SIZE];
    reqMessage.read(buffer, BUFFER_SIZE);  // Read the message
    std::cout << "reqMessage: " << buffer;

    if (comm.isTcp()) {  // If the communication is TCP, use TCP
        TCPInfo tcp(gsip, gsport); 
        tcp.send(reqMessage);         // send request message 
        resMessage = tcp.receive();   // receive response
    }
    
    else {  // If the communication is UDP, use UDP

        UDPInfo udp(gsip, gsport); 
        udp.send(reqMessage);         // request 

        resMessage = udp.receive();   // receive response
        
    }

    StreamMessage resStreamMessage(resMessage);  // Create a StreamMessage from the response
    
    comm.decodeResponse(resStreamMessage);  // Decode the response
    std::cout << "got response message ";
}