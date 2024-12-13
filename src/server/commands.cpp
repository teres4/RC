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
    std::cout << "handleCommand: " << message;

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

    std::cout << "in start command: " << args << response;

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
            startComm._status = "OK";
            result = "Player does not have an ongoing game";
            // need: PLID MODE TIME STARTDATE TIMESTART
            std::string current_datetime = currentDateTime();
            DB.createGame(std::to_string(startComm._plid), 'P', startComm._time, 
                            current_datetime, time_t(&current_datetime));
        }
    }
    catch (ProtocolException const &e)
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
    //     // TODO check verbose

    std::cout << args << response << receiver.isverbose();
}

void ExitCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose

    std::cout << args << response << receiver.isverbose();
}

void DebugCommand::handle(std::string &args, std::string &response, Server &receiver)
{
    //     // TODO check verbose

    std::cout << args << response << receiver.isverbose();
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


std::string currentDateTime(){
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