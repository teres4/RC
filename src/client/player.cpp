#include <iostream>
#include "player.hpp"
#include "commands.hpp"

extern bool is_exiting;

int main(int argc, char *argv[])
{
    setup_signal_handlers();   // change the signal handlers to our own
    Client client(argc, argv); // parse command-line arguments

    CommandManager commandManager; // create a new command manager
    commandManager.addAllCommands();

    // player info
    // setup sockets

    while (!std::cin.eof() && !is_exiting)
    {
        commandManager.waitForCommand(client);
    }

    if (client._player.activePlayer()){
        std::cout << "Player has an active game, will attempt to quit it." << std::endl;
        try {
            QuitCommunication quitComm;
            quitComm._plid = client._player.getPlid();

            client.processRequest(quitComm); // Send the request to the server, receiving its response

            if (quitComm._status == "OK")
            {
                std::cout << "Game has ended." << std::endl;
                std::cout << "The secret key: " << quitComm._key << std::endl;
                client._player.finishGame();
            }
            else
                std::cerr << "Failed to quit game. Exiting anyway." << std::endl;
        } 
        catch (...){
            std::cerr << "Encountered unrecoverable error while running the "
                            "application. Shutting down..." << std::endl;
            return EXIT_FAILURE;
        }
    } 
    return EXIT_SUCCESS;
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
    switch (argc) {
        case 1:
            return;
        case 3:
            if (strcmp(argv[1], "-n") == 0)
                _gsip = argv[2];
            else if (strcmp(argv[1], "-p") == 0)
                _gsport = argv[2];
            else {
                std::cout << "Wrong args\nCorrect usage: [-n GSIP] [-p GSport]\n";
                return;
            }
            break;
        case 5:
            if (strcmp(argv[1], "-n") == 0)
                _gsip = argv[2];
            else {
                std::cout << "Wrong args\nCorrect usage: [-n GSIP] [-p GSport]\n";
                return;
            }
            if (strcmp(argv[3], "-p") == 0)
                _gsport = argv[4];
            else {
                std::cout << "Wrong args\nCorrect usage: [-n GSIP] [-p GSport]\n";
                return;
            }
            break;
        default:
            throw UnrecoverableError("Invalid arguments.\nCorrect usage: [-n GSIP] [-p GSport]\n");
            return;
    }

    validate_port(_gsport);
}


void Client::processRequest(ProtocolCommunication &comm)
{
    std::string reqMessage = comm.encodeRequest();
    std::string resMessage = "";

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
                if (resMessage != "")           // response received
                    break;
            }
            catch (...) {
                if (triesLeft == 0)
                    throw;
            }
        }

    }

    StreamMessage resStreamMessage(resMessage); // Create a StreamMessage from the response

    comm.decodeResponse(resStreamMessage); // Decode the response
}


void Client::writeFile(std::string fName, std::string &data) {
    try {
        checkDir();  // Assure that the directory exists

        std::ofstream file(_path + fName);  // Create a file with the given name

        ssize_t n = (ssize_t) data.length();
        ssize_t total_written = 0;

        while (total_written < n) {
            file.write(data.c_str() + total_written, n - total_written);

            total_written += (ssize_t)file.tellp() - total_written;
        }

        file.close();  // Close the file
    }
    catch (std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
    }



}

void Client::checkDir() {
    if (mkdir(_path.c_str(), 0777) == -1) {  // If the directory doesn't exist, create it
        if (errno != EEXIST) { // The named file exists.
            throw std::runtime_error("Couldn't write file");
        }
    }
}