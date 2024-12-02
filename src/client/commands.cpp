#include "commands.hpp"
#include "player.hpp"
#include <iostream>

bool exit_command = false; // flag to indicate whether the application is exiting

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{
  _handlers[command->_name] = std::move(command);
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

  auto handler = _handlers.find(commandName); // find handler of the command

  if (handler == _handlers.end())
  { // If the handler does not exist
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

  if (arg_split.size() != 2)
  { // If the number of arguments is not 2
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg
              << std::endl
              << plid_error << max_playtime_error << std::endl;
    return;
  }

  // Get the username and password from the arguments
  std::string PLID = arg_split[0];
  std::string max_playtime = arg_split[1];

  if (validate_plid(PLID) == false)
  {
    std::cout << "Invalid PLID: " << plid_error << std::endl;
    return;
  }
  if (validatePlayTime(max_playtime) == false)
  {
    std::cout << "Invalid max_playtime: " << max_playtime_error << std::endl;
    return;
  }

  StartCommunication startComm;
  startComm._plid = get_plid(PLID);
  startComm._time = get_playtime(max_playtime);

  state.processRequest(startComm); // Send the request to the server, receiving its response

  if (startComm._status == "OK")
  {
    std::cout << "New game has started!" << std::endl;
    state._player.newPlayer(get_plid(PLID));
  }
  else if (startComm._status == "NOK")
  {
    std::cout << "Player already has an ongoing game" << std::endl;
    state._player.newPlayer(get_plid(PLID));
  }
  else if (startComm._status == "ERR")
  {
    std::cout << "Check syntax" << std::endl;
  }
}

void TryCommand::handle(std::string args, Client &state)
{
  if (!state._player.activePlayer())
  {
    std::cout << "There's not an active player" << std::endl;
    return;
  }

  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 4)
  {
    // If the number of arguments is not 4
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  for (auto c : arg_split)
  {
    if (validate_color(c) == false)
    {
      std::cout << "Invalid Color." << std::endl;
      return;
    }
  }

  TryCommunication tryComm;
  tryComm._plid = state._player.getPlid();
  tryComm._key = get_color_key(args);
  tryComm._nT = state._player.getnT();

  state.processRequest(tryComm); // Send the request to the server, receiving its response

  // TODO stuff if OK response?
  if (tryComm._status == "OK")
  {
    std::cout << "Number of trial: " << state._player.getnT() << std::endl
              << "Number of black: " << tryComm._nB << std::endl
              << "Number of white: "
              << tryComm._nW << std::endl;

    if (tryComm._nB == 4)
    {
      std::cout << "Game won!" << std::endl;
    }

    state._player.increaseNT();

    // enunciado stuff?
  }
  else if (tryComm._status == "DUP")
  {
    std::cout << "Duplicate of a previous trial's guess " << std::endl;
  }

  else if (tryComm._status == "INV")
  { // TODO - message
    std::cout << "" << std::endl;
  }
  else if (tryComm._status == "NOK")
  {
    std::cout << "Trial out of context" << std::endl;
  }
  else if (tryComm._status == "ENT")
  {
    std::cout << "No more attempts available. Secret key was: " << tryComm._key
              << std::endl;
  }
  else if (tryComm._status == "ETM")
  {
    std::cout << "Maximum play time has been exceeded. Secret key was: "
              << tryComm._key << std::endl;
  }
  else if (tryComm._status == "ERR")
  {
    std::cout << "Check syntax" << std::endl;
  }
}

void ShowTrialsCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state._player.getPlid();

  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }
}

void ScoreboardCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state._player.getPlid();
}

void QuitCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state._player.getPlid();

  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  QuitCommunication quitComm;

  quitComm._plid = state._player.getPlid();

  state.processRequest(quitComm); // Send the request to the server, receiving its response

  if (quitComm._status == "OK")
  {
    std::cout << "Game has ended." << std::endl;
    std::cout << "The secret key: " << quitComm._key << std::endl;
    state._player.finishGame();
  }
  else if (quitComm._status == "NOK")
  {
    std::cout << "Player has no ongoing game." << std::endl;
  }
  else if (quitComm._status == "ERR")
  {
    std::cout << "Check syntax" << std::endl;
  }

  exit_command = true;
}

void ExitCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state._player.getPlid();

  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  QuitCommunication quitComm;

  quitComm._plid = state._player.getPlid();

  state.processRequest(quitComm); // Send the request to the server, receiving its response

  if (quitComm._status == "OK")
  {
    std::cout << "Game has ended." << std::endl;
    std::cout << "The secret key:" << quitComm._key << std::endl;
  }
  else if (quitComm._status == "NOK")
  {
    std::cout << "Player has no ongoing game." << std::endl;
  }
  else if (quitComm._status == "ERR")
  {
    std::cout << "Check syntax" << std::endl;
  }

  // make the player programn finish

  exit_command = true;
  exit(0);
}

void DebugCommand::handle(std::string args, Client &state)
{
  // TODO
  std::cout << args << state._player.getPlid();
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