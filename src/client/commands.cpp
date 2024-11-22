#include "commands.hpp"
#include <iostream>


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
  std::cout << "> ";   // Print the prompt

  std::string input;
  std::getline(std::cin, input); // Read the line from the user

  if (std::cin.eof()) // in case of ctrl-d 
      return;

  std::stringstream ss(input);  // Create a stringstream object
  std::string temp;
  std::vector<std::string> command_split;

  // Splitting the input string by space
  while (std::getline(ss, temp, ' '))
      command_split.push_back(temp);

  if (command_split.size() == 0)
    return;

  std::string commandName = command_split[0];   // The name of the command

  if (commandName.length() == 0)
      return;
  

  auto handler = handlers.find(commandName); // handler of the command

  if (handler == handlers.end()) { // If the handler does not exist
    std::cout << "Invalid command: " << commandName << std::endl;
    return;
  }

  try {
    input.erase(0, commandName.length());
    handler->second->handle(input, state);
  } catch (std::exception& e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
  } catch (...) {
    std::cout << "[ERROR] An unknown error occurred." << std::endl;
  }

  return;

}


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