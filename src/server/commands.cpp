#include "commands.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> command)
{
    try
    {
        _handlers.insert({command->_name, command});
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return;
}

void CommandManager::registerAllCommands()
{
    this->registerCommand(std::make_shared<StartCommand>());
    this->registerCommand(std::make_shared<TryCommand>());
    this->registerCommand(std::make_shared<QuitCommand>());
    this->registerCommand(std::make_shared<ShowTrialsCommand>());
    this->registerCommand(std::make_shared<ScoreboardCommand>());
    this->registerCommand(std::make_shared<ExitCommand>());
    this->registerCommand(std::make_shared<DebugCommand>());
}

std::string CommandManager::handleCommand(std::string message, Server &receiver)
{
    std::vector<std::string> command_split = split_command(message);

    if (command_split.size() == 0)
        return "ERR";

    std::string commandName = command_split[0]; // The name of the command

    if (commandName.length() == 0 || commandName.length() != 3)
        return "ERR";

    auto handler = _handlers.find(commandName); // find handler of the command

    if (handler == _handlers.end())
    { // If the handler does not exist
        handler = _handlers.find(commandName);
        if (handler == _handlers.end())
        {
            std::cout << "Invalid command: " << commandName << std::endl;
            return "ERR";
        }
    }

    std::string response;

    handler->second->handle(message, response, receiver);

    return response;
}

void StartCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    StartCommunication startComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        startComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(startComm._plid));
        if (hasGame)
        {
            startComm._status = "NOK";
            result = "Player has an ongoing game";
        }
        else
        {
            // need: PLID MODE TIME STARTDATE TIMESTART
            std::string current_datetime = currentDateTime();
            DB.createGame(std::to_string(startComm._plid), 'P', startComm._time,
                          current_datetime, time_t(&current_datetime));
            startComm._status = "OK";
            result = "Player started a game. ";              
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        startComm._status = "ERR";
        result = "Protocol Error";
    }
    response = startComm.encodeResponse(); // Encode the response
    return;
}

void TryCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose

    std::cout << args << response << receiver.isverbose();

    GamedataManager DB = receiver._DB;

    // TRY PLID C1 C2 C3 C4 nT

    // RTR status [nT nB nW][C1 C2 C3 C4]

    TryCommunication tryComm;

    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        tryComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(tryComm._plid));
        if (hasGame)
        {
            tryComm._status = "NOK";
            result = "Player has an ongoing game";
        }
        else
        {
                }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        tryComm._status = "ERR";
        result = "Protocol Error";
    }
}

void ShowTrialsCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose

    std::cout << args << response << receiver.isverbose();
}

void ScoreboardCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose
    std::cout << args << response << receiver.isverbose();
}

void QuitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    QuitCommunication quitComm;
    std::string result;
    try
    {
        StreamMessage reqMessage(args);
        quitComm.decodeRequest(reqMessage); // Decode the request

        bool hasGame = DB.hasOngoingGame(std::to_string(quitComm._plid));
        if (hasGame) // exit game
        {
            // EXIT THE GAME
            quitComm._status = "OK";  // Set the status to OK if everything goes right
            quitComm._key = DB.getsecretKey(std::to_string(quitComm._plid));
            result = "Game has ended sucessfully";
        }
        else
        {
            quitComm._status = "NOK";
            result = "Player does not have an ongoing game";
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, set the status to ERR
        quitComm._status = "ERR";
        result = "Protocol Error";
    }
    response = quitComm.encodeResponse(); // Encode the response
    return;
}

void ExitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    GamedataManager DB = receiver._DB;

    QuitCommunication quitComm; 
    std::string result;
    try {
        StreamMessage reqMessage(args);
        quitComm.decodeRequest(reqMessage);  // Decode the request

        bool hasGame = DB.hasOngoingGame(std::to_string(quitComm._plid)); 
        if (hasGame) // exit game
        {
            // EXIT THE GAME
            quitComm._status = "OK";  // Set the status to OK if everything goes right
            // TODO quitComm._key = 
            result = "Game has ended sucessfully";
        }
        else
        {
            quitComm._status = "NOK";
            result = "Player does not have an ongoing game";

        }
    
    } catch (ProtocolException &e) {  // If the protocol is not valid, set the status to ERR
        quitComm._status = "ERR";
        result = "Protocol Error";
    }
    response = quitComm.encodeResponse();  // Encode the response
    // DELETE ALL INFO FROM PLAYER
    return;
}

void DebugCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    // TODO check verbose
    GamedataManager DB = receiver._DB;

    DebugCommunication dbgComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(args);
        dbgComm.decodeRequest(reqMessage); // Decode the request

        // check database if player has an ongoing game
        bool hasGame = DB.hasOngoingGame(std::to_string(dbgComm._plid));
        if (hasGame)
        {
            dbgComm._status = "NOK";
            result = "Player has an ongoing game";
        }
        else
        {
            dbgComm._status = "OK";
            result = "Player does not have an ongoing game";

            std::string current_datetime = currentDateTime();
            DB.createGame(std::to_string(dbgComm._plid), 'D', removeSpaces(dbgComm._key), dbgComm._time,
                          current_datetime, time_t(&current_datetime));
        }
    }
    catch (ProtocolException &e)
    { // If the protocol is not valid, status = "ERR"
        dbgComm._status = "ERR";
        result = "Protocol Error";
    }
    response = dbgComm.encodeResponse(); // Encode the response
    return;
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

std::string currentDateTime()
{
    time_t fulltime;
    struct tm *current_time;
    char time_str[50];

    time(&fulltime);
    current_time = gmtime(&fulltime);
    sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", current_time->tm_year + 1900,
            current_time->tm_mon + 1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    return time_str;
}

std::string removeSpaces(std::string &str)
{
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}
