#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "../common/constants.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    Server server(argc, argv);

    // open conections
    server.InitializeServers();

    return 0;
}

Server::Server(int argc, char **argv)
{

    if (argc == 4)
    {
        if (strcmp(argv[1], "-p"))
        {
            _gsport = argv[2];
        }
        if (strcmp(argv[3], "-v"))
        {
            _verbose = true;
        }
        else
        {
            std::cout << "Wrong args/n Correct usage: [-p GSport] [-v]";
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-p"))
        {
            _gsport = argv[2];
        }
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-v"))
        {
            _verbose = true;
        }
    }
    else
        std::cout << "Wrong args/n Correct usage: [-p GSport] [-v]";
}

// setup sockets
void Server::InitializeServers()
{ /*
  try {                                       // Try to start the server
      UdpServer udpServer(server.getPort());  // Initialize the UDP server
      TcpServer tcpServer(server.getPort());  // Initialize the TCP server

      // Display the server information if verbose mode is enabled
      server.logPush("Listening on port " + server.getPort());
      if ((pid = fork()) == -1) {  // Fork the process
          exit(1);
      } else if (pid == 0) {  // If the process is a child process
          tcpServer.close();  // Close the TCP server
          server.logPush("UDP server started");   // Display a message if
                                                  // verbose mode is enabled
          UDPServer(udpServer, manager, server);  // Start the UDP server
      } else {                // If the process is a parent process
          udpServer.close();  // Close the UDP server
          server.logPush("TCP server started");   // Display a message if
                                                  // verbose mode is enabled
          TCPServer(tcpServer, manager, server);  // Start the TCP server
      }
  */
    try
    {
        UdpServer UdpServer(_gsport);
        TcpServer TcpServer(_gsport);

        int pid = fork();
        if (pid > 0) // parent process
        {
            // udp
            UdpServer.closeServer();
        }
        else if (pid == 0) // child process
        {
            UdpServer.closeServer();
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