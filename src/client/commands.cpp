#include "commands.hpp"
#include <iostream>

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

  uint32_t player_id;

  // parse plid
  // validate plid
  // parse max time


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