#include "commands.hpp"

// void start(std::string args)
// {
//     // validate plid
//     // validate max_playtime
//     struct addrinfo hints;

//     std::string plid = args.substr(0, args.find(" "));
//     int max_playtime = std::stoi(args.substr(args.find(" ") + 1));

//     int plid_int = validate_plid(plid);
//     if (plid_int == INVALID)
//     {
//         std::cerr << "Invalid PLID" << std::endl;
//         exit(1);
//     }

//     int playtime_int = validatePlayTime(std::to_string(max_playtime));
//     if (playtime_int == INVALID)
//     {
//         std::cerr << "Invalid playtime" << std::endl;
//         exit(1);
//     }

//     // create a socket
//     int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0)
//     {
//         std::cerr << "Error creating socket" << std::endl;
//         exit(1);
//     }

//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_INET; // IPv4
//     hints.ai_socktype = SOCK_DGRAM;

//         int errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
//     if (errcode != 0) /*error */
//     {
//         std::cerr << "Error getting address info" << std::endl;
//         exit(1);
//     }
// }
// Create datagram with server IP and port=x; send datagram via clientSocket

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{
    handlers[command->name] = std::move(command);
}

void CommandManager::printHelp()
{
    for (auto &handler : handlerList)
    {
        std::cout << handler->name << " " << handler->args.value_or("") << " - " << handler->usage.value_or("") << std::endl;
    }
}

void CommandManager::waitForCommand(Player_Info &state)
{
    // Add your implementation here
}