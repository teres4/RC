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

// void CommandManager::printHelp()
// {
//     for (auto &handler : handlerList)
//     {
//         std::cout << handler->name << " " << handler->args.value_or("") << " - " << handler->usage.value_or("") << std::endl;
//     }
// }

void CommandManager::waitForCommand(Player_Info &state)
{
    std::string input;

    std::cout << "> ";   // Print the prompt
    // std::getline(std::cin, input); // Gets command

    while (getline(std::cin, input, ' ' )){

    }


    if (std::cin.eof()) // If the user has pressed Ctrl+D
        return;

    std::cout << state.hasActiveGame;



}


// {
//   std::string commandName;          // The name of the command
//   std::string args;                 // The arguments to the command
//   auto splitIndex = line.find(' '); // Find the index of the separator space

//   if (splitIndex == std::string::npos) { // If there is no space
//     commandName = line;                  // The entire line is the command name
//     args = "";                           // There are no arguments
//   } else {
//     commandName = line.substr(0, splitIndex);
//     args = line.erase(0, splitIndex + 1);
//   }

//   if (commandName.length() == 0) { // If the command name is empty
//     return;
//   }

//   auto handler = handlers.find(commandName); // Find the handler for the command

//   if (handler == handlers.end()) { // If the handler does not exist
//     std::cout << "Invalid command: " << commandName << std::endl;
//     return;
//   }

//   try {
//     handler->second->handleCommand(args, state); // Handle the command
//   } catch (std::exception &e) {
//     std::cout << "Error: " << e.what() << std::endl;
//   }
// }



void StartCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}

void TryCommand::handle(std::string args, Player_Info& state) {

// TODO
std::cout << args << state.hasActiveGame;

}

void ShowTrialsCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}

void ScoreboardCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}

void QuitCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}

void ExitCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}

void DebugCommand::handle(std::string args, Player_Info& state) {
  // TODO
  std::cout << args << state.hasActiveGame;

}