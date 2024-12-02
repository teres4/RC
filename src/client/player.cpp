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

    // player info
    // setup sockets

    while (!std::cin.eof())
    {
        commandManager.waitForCommand(client);
    }

    return 0;
}

void Player::newPlayer(int plid)
{
    _plid = plid;
    _nT = 1;
    onGoing = true;
}

int Player::getPlid()
{
    return _plid;
}

int Player::getnT()
{
    return _nT;
}

bool Player::activePlayer()
{
    return onGoing;
}

void Player::increaseNT()
{
    _nT++;
}

void Player::finishGame()
{
    onGoing = false;
    _nT = 1;
    _plid = -1;
}

Client::Client(int argc, char **argv)
{
    if (argc == 3)
    { // check if -n or -p
        if (strcmp(argv[1], "-n") == 0)
            _gsip = argv[2];

        else if (strcmp(argv[1], "-p") == 0)
            _gsport = argv[2];
    }

    if (argc == 5)
    {
        _gsip = argv[2];
        _gsport = argv[4];
    }

    validate_port(_gsport);
}

void Client::processRequest(ProtocolCommunication &comm)
{
    std::string reqMessage = comm.encodeRequest();
    std::string resMessage;

    std::cout << "processReq message: " << reqMessage;

    if (comm.isTcp())
    { // If the communication is TCP, use TCP
        TCPInfo tcp(_gsip, _gsport);
        tcp.send(reqMessage);       // send request message
        resMessage = tcp.receive(); // receive response
    }

    else
    { // If the communication is UDP, use UDP
        UDPInfo udp(_gsip, _gsport);
        udp.send(reqMessage);       // request
        resMessage = udp.receive(); // receive response
    }

    StreamMessage resStreamMessage(resMessage); // Create a StreamMessage from the response
    std::cout << resMessage;

    comm.decodeResponse(resStreamMessage); // Decode the response
}