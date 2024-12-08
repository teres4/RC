#include "commands.hpp"

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{

    // _handlers[command->_name] = std::move(command);

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