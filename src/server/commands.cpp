#include "commands.hpp"

bool exiting = false;

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

std::string CommandManager::handleCommand(std::string message)
{
    std::cout << "handleCommand: " << message << std::endl;

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

    handler->second->handle(message, response);

    return response;
}

void StartCommand::handle(std::string &message, std::string &response)
{
    // TODO check verbose

    std::cout << "in start command: " << message << response << std::endl;

    StartCommunication startComm;
    std::string result;

    try
    {
        StreamMessage reqMessage(message);
        startComm.decodeRequest(reqMessage); // Decode the request

        // check if player has an ongoing game
        // database add player
        // startComm._status = "OK" or "NOK"
    }
    catch (ProtocolException const &e)
    { // If the protocol is not valid, status = "ERR"
        startComm._status = "ERR";
        result = "Protocol Error";
    }
    response = startComm.encodeResponse(); // Encode the response
}

void TryCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose

    std::cout << args << response;
}

void ShowTrialsCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose

    std::cout << args << response;
}

void ScoreboardCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose
    std::cout << args << response;
}

void QuitCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose

    std::cout << args << response;
}

void ExitCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose

    std::cout << args << response;
}

void DebugCommand::handle(std::string &args, std::string &response)
{
    //     // TODO check verbose

    std::cout << args << response;
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
