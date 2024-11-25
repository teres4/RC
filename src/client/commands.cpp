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
  std::cout << "> "; // Print the prompt

  std::string input;
  std::getline(std::cin, input); // Read the line from the user

  if (std::cin.eof()) // in case of ctrl-d
    return;

  std::vector<std::string> command_split = split_command(input);

  if (command_split.size() == 0)
    return;

  std::string commandName = command_split[0]; // The name of the command

  if (commandName.length() == 0)
    return;

  auto handler = handlers.find(commandName); // find handler of the command

  if (handler == handlers.end()) { // If the handler does not exist
    std::cout << "Invalid command: " << commandName << std::endl;
    return;
  }

  try
  {
    input.erase(0, commandName.length() + 1); // gets args
    handler->second->handle(input, state);    // Call the handle function of the handler
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
}

void StartCommand::handle(std::string args, Player_Info &state)
{
  std::vector<std::string> arg_split = split_command(args);

  std::string plid_error = "PLID must be a 6-digit number.";
  std::string max_playtime_error = "max_playtime cannot exceed 600 seconds.";

  if (arg_split.size() != 2) {
    // If the number of arguments is not 2 
    std::cout << "Invalid number of arguments.\nUsage: " << *command_arg 
      << std::endl << plid_error << max_playtime_error << std::endl;
  }

  // Get the username and password from the arguments
  std::string PLID = arg_split[0];
  std::string max_playtime = arg_split[1];

  if (validate_plid(PLID) == INVALID){
    std::cout << "Invalid PLID: " << plid_error << std::endl;
  }
  if (validatePlayTime(max_playtime) == INVALID){
    std::cout << "Invalid max_playtime: " << max_playtime_error << std::endl;
  }

  state.plid = validate_plid(PLID);
  state.max_playtime = validatePlayTime(max_playtime);

  std::string message = "SNG " + PLID + " " + max_playtime;

  if (sendto(state.udpSockFD, message.c_str(), message.length(), 0, state.serverUdpAddr->ai_addr, state.serverUdpAddr->ai_addrlen) == -1)
  {
    throw std::runtime_error("Error sending message to server");
  }
}

void TryCommand::handle(std::string args, Player_Info& state) {
  if (!state.hasActiveGame){
    return;
  }
  std::string plid_error = "PLID must be a 6-digit number.";
  std::string max_playtime_error = "max_playtime cannot exceed 600 seconds.";

  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 2) {    // If the number of arguments is not 2, throw exception
    // throw StartCommandArgumentException(*command_arg);
  }


// TODO
std::cout << args << state.hasActiveGame;

  // TODO
  std::cout << args << state.hasActiveGame;
}

void ShowTrialsCommand::handle(std::string args, Player_Info &state)
{
  // TODO
  std::cout << args << state.hasActiveGame;
}

void ScoreboardCommand::handle(std::string args, Player_Info &state)
{
  // TODO
  std::cout << args << state.hasActiveGame;
}

void QuitCommand::handle(std::string args, Player_Info &state)
{
  // TODO
  std::cout << args << state.hasActiveGame;
}

void ExitCommand::handle(std::string args, Player_Info &state)
{
  // TODO
  std::cout << args << state.hasActiveGame;
}

void DebugCommand::handle(std::string args, Player_Info &state)
{
  // TODO
  std::cout << args << state.hasActiveGame;
}

std::vector<std::string> split_command(std::string input)
{
  std::stringstream ss(input); // Create a stringstream object
  std::string temp;
  std::vector<std::string> command_split;

  // Splitting the input string by space
  while (std::getline(ss, temp, ' '))
    command_split.push_back(temp);

  return command_split;
}