#include <iostream>
#include "player.hpp"
#include "commands.hpp"

extern bool exit_command;

int main(int argc, char *argv[])
{
    setup_signal_handlers();   // change the signal handlers to our own
    Client client(argc, argv); // parse command-line arguments

    CommandManager commandManager; // create a new command manager
    commandManager.addAllCommands();

    // player info
    // setup sockets

    while (!std::cin.eof() && !exit_command)
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
    _plid = 0;
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

    if (comm.isTcp())   // If the communication is TCP, use TCP
    { 
        TCPInfo tcp(_gsip, _gsport);
        try {
            tcp.send(reqMessage);           // send request message
            resMessage = tcp.receive();     // receive response
        } catch (...) {
            tcp.closeTcpSocket();
        }
        tcp.closeTcpSocket();
    }

    else    // If the communication is UDP, use UDP
    { 
        UDPInfo udp(_gsip, _gsport);
        int triesLeft = RESEND_TRIES;
        while (triesLeft > 0) {
            --triesLeft;
            try {
                udp.send(reqMessage);           // send request message
                resMessage = udp.receive();     // receive response
            }
            catch (...) {
                if (triesLeft == 0)
                    throw;
            }
        }

    }

    StreamMessage resStreamMessage(resMessage); // Create a StreamMessage from the response

    // std::cout << resMessage;

    comm.decodeResponse(resStreamMessage); // Decode the response
}


void Client::writeFile(std::string fName, std::string &data) {
    checkDir();  // Assure that the directory exists

    std::ofstream file(_path + fName);  // Create a file with the given name

    ssize_t n = (ssize_t) data.length();
    file.write(data.c_str(), n);

    file.close();  // Close the file
}


void Client::checkDir() {
    if (mkdir(_path.c_str(), 0777) == -1) {  // If the directory doesn't exist, create it
        if (errno != EEXIST) { // The named file exists.
            throw std::runtime_error("Couldn't write file");
        }
    }
}