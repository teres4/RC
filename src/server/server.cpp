#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdexcept>
#include <fcntl.h>

#include "server.hpp"
#include "commands.hpp"

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server);

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server);

extern bool is_exiting;

int main(int argc, char *argv[])
{
    try
    {
        setup_signal_handlers();
        Server server(argc, argv);

        CommandManager commandManager; // create a new command manager
        commandManager.registerAllCommands();

        while (!is_exiting)
        {
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
                    UDPServer(udpServer, commandManager, server); // start udp server
                }
                else // parent process
                {
                    udpServer.closeServer();
                    TCPServer(tcpServer, commandManager, server);
                }
            }
            catch (ProtocolException &e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            catch (UnrecoverableError &e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                is_exiting = true; // Trigger shutdown
                break;             // Exit loop
            }
            catch (SocketException &e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                is_exiting = true; // Trigger shutdown
                break;             // Exit loop
            }
            catch (...)
            {
                std::cerr << "Encountered unrecoverable error while running the "
                             "application. Shutting down..."
                          << std::endl;
                is_exiting = true; // Trigger shutdown
                break;             // Exit loop
            }
        }
        // if exiting, finish all games
        if (is_exiting)
            server._DB.quitAllGames();
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        is_exiting = true; // Ensure shutdown if a fatal exception escapes the loop
    }
    catch (...)
    {
        std::cerr << "Encountered unrecoverable error while running the "
                     "application. Shutting down..."
                  << std::endl;
    }
    return EXIT_SUCCESS;
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

    while (!is_exiting)
    {
        std::string message = udpServer.receive();
        std::string response = manager.handleCommand(message, server);

        if (verbose)
        {
            std::cout << "Client IP: " << udpServer.getClientIP() << std::endl;
            std::cout << "Client port: " << udpServer.getClientPort() << std::endl
                      << std::endl;
        }
        udpServer.send(response);
    }
}

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server)
{
    bool verbose = server.isverbose();

    int newfd, pid, ret;
    char buffer[128];
    struct sockaddr_in addr;
    char *ptr;
    ssize_t n, nw;
    std::string message;

    while (!is_exiting)
    {
        socklen_t addrlen = sizeof(addr);
        do
            newfd = accept(tcpServer._fd, (struct sockaddr *)&addr, &addrlen);

        while (newfd == -1 && errno == EINTR && !is_exiting);
        if (is_exiting)
        {
            close(tcpServer._fd);
            return;
        }

        if (newfd == -1) // error
            exit(1);

        struct timeval read_timeout;
        read_timeout.tv_sec = TCP_READ_TIMEOUT;
        read_timeout.tv_usec = 0;
        if (setsockopt(newfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) < 0) 
            throw SocketException();

        struct timeval write_timeout;
        write_timeout.tv_sec = TCP_WRITE_TIMEOUT;
        write_timeout.tv_usec = 0;
        if (setsockopt(newfd, SOL_SOCKET, SO_SNDTIMEO, &write_timeout, sizeof(write_timeout)) < 0) 
            throw SocketException();

        if ((pid = fork()) == -1) // error
            exit(1);
        else if (pid == 0) // child
        {
            {
                close(tcpServer._fd);
                n = read(newfd, buffer, 128);
                if (n == -1) // error
                    exit(1);

                // add buffer to string
                message.append(buffer, (size_t)n);

                std::string response = manager.handleCommand(message, server);

                ptr = (char *)response.c_str();
                n = static_cast<ssize_t>(response.size());
                while (n > 0)
                {
                    if ((nw = write(newfd, ptr, (size_t)n)) <= 0) // error
                        exit(1);
                    n -= nw;
                    ptr += nw;
                }
                if (verbose)
                {
                    std::cout << "Client IP: " << tcpServer.getClientIP() << std::endl;
                    std::cout << "Client port: " << tcpServer.getClientPort() << std::endl
                              << std::endl;
                }
                close(newfd);
                exit(0);
            }
        }

        do
            ret = close(newfd);
        while ((ret == -1) && (errno = EINTR));
        if (ret == -1) // error
            exit(1);
    }

}
