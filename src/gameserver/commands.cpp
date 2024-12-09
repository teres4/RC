#include "commands.hpp"

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
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
std::string CommandManager::handleCommand(std::string message, Server &server)
{
    std::vector<std::string> command_split = split_command(message);

    if (command_split.size() == 0)
        return;

    std::string commandName = command_split[0]; // The name of the command

    if (commandName.length() == 0)
        return;

    auto handler = _handlers.find(commandName); // find handler of the command

    if (handler == _handlers.end())
    { // If the handler does not exist
        handler = _handlers.find(commandName);
        if (handler == _handlers.end())
        {
            std::cout << "Invalid command: " << commandName << std::endl;
            return;
        }
    }

    std::string args = "";
    if (command_split.size() > 1)
        args = command_split[1];

    handler->second->handle(args, server);
}