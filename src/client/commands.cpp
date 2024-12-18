#include "commands.hpp"
#include "player.hpp"
#include <iostream>

extern bool is_exiting; // flag to indicate whether the application is exiting

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{
  _handlers.insert({command->_name, command});

  if (command->_alias != "")
  {
    _handlers.insert({command->_alias, command});
  }
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
    std::cerr << "Invalid command: " << commandName << std::endl;
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
  catch (...)
  {
    std::cout << "[ERROR] An unknown error occurred." << std::endl;
  }
}

void StartCommand::handle(std::string args, Client &state)
{
  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 2)
  { // If the number of arguments is not 2
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  // Get the username and password from the arguments
  std::string PLID = arg_split[0];
  std::string max_playtime = arg_split[1];

  try
  {
    validate_plid(PLID);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid PLID: PLID must be a 6-digit number." << std::endl;
    return;
  }

  try
  {
    validate_playTime(max_playtime);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid max_playtime: max_playtime cannot exceed 600 seconds."
              << std::endl;
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
    std::cout << "Game has failed to start. Please check syntax. " << std::endl;
  }
}

void TryCommand::handle(std::string args, Client &state)
{
  if (!state._player.activePlayer())
  {
    std::cout << "There's not an active player" << std::endl;
    return;
  }
  if (!state._player.activeGame()){
    std::cout << "There's not an active game" << std::endl;
    return;
  }

  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 4)
  {
    // If the number of arguments is not 4
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  try
  {
    validate_key(arg_split);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid color key." << std::endl;
    return;
  }

  TryCommunication tryComm;
  tryComm._plid = state._player.getPlid();
  tryComm._key = get_color_key(args);
  tryComm._nT = state._player.getnT();

  state.processRequest(tryComm); // Send the request to the server, receiving its response

  if (tryComm._status == "OK" || tryComm._status == "ENT" || tryComm._status == "ETM")
  {
    std::cout << "Number of trial: " << state._player.getnT() << std::endl
              << "Number of black: " << tryComm._nB << std::endl
              << "Number of white: "
              << tryComm._nW << std::endl;

    if (tryComm._nB == 4)
    {
      std::cout << "Game won!" << std::endl;
      state._player.finishGame();
    }

    state._player.increaseNT();

  }
  if (tryComm._status == "DUP")
    std::cout << "Duplicate of a previous trial's guess " << std::endl;

  if (tryComm._status == "INV")
    std::cout << "INV" << std::endl;

  if (tryComm._status == "NOK")
    std::cout << "Trial out of context" << std::endl;

  if (tryComm._status == "ENT")
  {
    std::cout << "No more attempts available. Secret key was: " << tryComm._key
              << std::endl;
    state._player.finishGame();
  }

  if (tryComm._status == "ETM")
  {
    std::cout << "Maximum play time has been exceeded. Secret key was: "
              << tryComm._key << std::endl;
    state._player.finishGame();
  }

  if (tryComm._status == "ERR")
    std::cout << "Failed at try request. Please check syntax. " << std::endl;
}

void QuitCommand::handle(std::string args, Client &state)
{
  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  if (!state._player.activePlayer())
  {
    std::cout << "There's not an active player" << std::endl;
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
    std::cout << "Player had no ongoing game." << std::endl;
    state._player.setOngoing(false);
  }
  else if (quitComm._status == "ERR")
  {
    std::cout << "Failed to quit." << std::endl;
  }
}

void ExitCommand::handle(std::string args, Client &state)
{
  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  if (state._player.activePlayer())
  {
    QuitCommunication quitComm;

    quitComm._plid = state._player.getPlid();

    state.processRequest(quitComm); // Send the request to the server, receiving its response

    if (quitComm._status == "OK")
    {
      std::cout << "Game has ended." << std::endl;
      std::cout << "The secret key:" << quitComm._key << std::endl;
      state._player.quitPlayer();
    }
    else if (quitComm._status == "NOK")
    {
      state._player.setOngoing(false);
    }
    else if (quitComm._status == "ERR")
    {
      std::cout << "Failed to exit." << std::endl;
    }
  }
  // make the player programn finish
  is_exiting = true;
  std::cout << "Exiting program " << std::endl;
}

void DebugCommand::handle(std::string args, Client &state)
{
  std::vector<std::string> arg_split = split_command(args);

  if (arg_split.size() != 6)
  {
    // If the number of arguments is not 6
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  std::string PLID = arg_split[0];
  std::string max_playtime = arg_split[1];

  try
  {
    validate_plid(PLID);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid PLID: PLID must be a 6-digit number." << std::endl;
    return;
  }

  try
  {
    validate_playTime(max_playtime);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid max_playtime: max_playtime cannot exceed 600 seconds."
              << std::endl;
    return;
  }

  std::vector<std::string> colorArray(arg_split.begin() + 2, std::end(arg_split));

  try
  {
    validate_key(colorArray);
  }
  catch (ProtocolException &e)
  {
    std::cerr << "Invalid color key." << std::endl;
    return;
  }

  DebugCommunication dbgComm;

  dbgComm._plid = get_plid(PLID);
  dbgComm._time = get_playtime(max_playtime);
  dbgComm._key = get_color_key(colorArray);

  state.processRequest(dbgComm);

  if (dbgComm._status == "OK")
  {
    std::cout << "New game has started in debug mode with key: " << dbgComm._key << std::endl;
    state._player.newPlayer(get_plid(PLID));
  }
  else if (dbgComm._status == "NOK")
  {
    std::cout << "Player already has an ongoing game" << std::endl;
    state._player.newPlayer(get_plid(PLID));
  }
  else if (dbgComm._status == "ERR")
  {
    std::cout << "Failed debug request. Please check syntax." << std::endl;
  }
}

void ShowTrialsCommand::handle(std::string args, Client &state)
{
  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  if (!state._player.activePlayer())
  {
    std::cout << "There's not an active player" << std::endl;
    return;
  }

  ShowTrialsCommunication stComm;
  stComm._plid = state._player.getPlid();

  state.processRequest(stComm); // Send the request to the server, receiving its response

  if (stComm._status == "ACT" || stComm._status == "FIN")
  {
    if (stComm._Fname.size() > MAX_FNAME)
    {
      std::cout << "Invalid filename - max of 24 alphanumerical characters";
      return;
    }
    if (stComm._Fsize > MAX_FSIZE)
    {
      std::cout << "File too big.";
      return;
    }
    std::cout << "Information stored in " << stComm._Fname << std::endl;
    std::cout << "Size of file (bytes): " << stComm._Fsize << std::endl;
    std::cout << stComm._Fdata << std::endl;
    state.writeFile(stComm._Fname, stComm._Fdata);
  }

  else if (stComm._status == "NOK")
    std::cout << "Player has no games." << std::endl;
}

void ScoreboardCommand::handle(std::string args, Client &state)
{
  if (args != "")
  {
    std::cout << "Invalid number of arguments.\nUsage: " << *_command_arg << std::endl;
    return;
  }

  ScoreboardCommunication sbComm;

  state.processRequest(sbComm); // Send the request to the server, receiving its response

  if (sbComm._status == "OK")
  {
    if (sbComm._Fname.size() > MAX_FNAME)
    {
      std::cout << "Invalid filename - max of 24 alphanumerical characters";
      return;
    }
    if (sbComm._Fsize > MAX_FSIZE)
    {
      std::cout << "File too big.";
      return;
    }
    std::cout << "Information stored in " << sbComm._Fname << std::endl;
    std::cout << "Size of file (bytes): " << sbComm._Fsize << std::endl;
    std::cout << sbComm._Fdata << std::endl;
    state.writeFile(sbComm._Fname, sbComm._Fdata);
  }
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
