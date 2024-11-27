#include "commands.hpp"
#include "player.hpp"
#include <iostream>

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{
  handlers[command->name] = std::move(command);
}


void CommandManager::addAllCommands()
{
  this->addCommand(std::make_shared<StartCommand>());
  this->addCommand(std::make_shared<TryCommand>());
  this->addCommand(std::make_shared<QuitCommand>());
  this->addCommand(std::make_shared<ShowTrialsCommand>());
  this->addCommand(std::make_shared<ScoreboardCommand>());
  this->addCommand(std::make_shared<ExitCommand>());
  this->addCommand(std::make_shared<DebugCommand>());
}


// void CommandManager::printHelp()
// {
//     for (auto &handler : handlerList)
//     {
//         std::cout << handler->name << " " << handler->args.value_or("") << " - " << handler->usage.value_or("") << std::endl;
//     }
// }

void CommandManager::waitForCommand(Client &state)
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



void StartCommand::handle(std::string args, Client &state)
{
  std::vector<std::string> arg_split = split_command(args);

  std::string plid_error = "PLID must be a 6-digit number.";
  std::string max_playtime_error = "max_playtime cannot exceed 600 seconds.";

  if (arg_split.size() != 2) {
    // If the number of arguments is not 2 
    std::cout << "Invalid number of arguments.\nUsage: " << *command_arg 
      << std::endl << plid_error << max_playtime_error << std::endl;
    return;
  }

  // Get the username and password from the arguments
  std::string PLID = arg_split[0];
  std::string max_playtime = arg_split[1];

  if (validate_plid(PLID) == INVALID){
    std::cout << "Invalid PLID: " << plid_error << std::endl;
    return;
  }
  if (validatePlayTime(max_playtime) == INVALID){
    std::cout << "Invalid max_playtime: " << max_playtime_error << std::endl;
    return;
  }

  StartCommunication startComm;
  startComm._plid = get_plid(PLID);
  startComm._time = get_playtime(max_playtime);

  state.processRequest(startComm);  // Send the request to the server, receiving its response


  if (startComm._status == "OK") {
    std::cout << "New game has started!" << std::endl;
    // start a new game
  } 
  else if (startComm._status == "NOK") {
    std::cout << "Player already has an ongoing game" << std::endl;
  } 
  else if (startComm._status == "ERR") {
    std::cout << "Check syntax" << std::endl;
  }

}





void TryCommand::handle(std::string args, Client& state) {
  // if (!state.hasActiveGame){
  //   return;
  // }
  std::string plid_error = "PLID must be a 6-digit number.";
  std::string max_playtime_error = "max_playtime cannot exceed 600 seconds.";

  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 2) {    // If the number of arguments is not 2, throw exception
    // throw StartCommandArgumentException(*command_arg);
  }


// TODO
std::cout << args << state.gsip;

  // TODO
  std::cout << args << state.gsip;
}

void ShowTrialsCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state.gsip;
}

void ScoreboardCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state.gsip;
}

void QuitCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state.gsip;
}

void ExitCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state.gsip;
}

void DebugCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state.gsip;
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