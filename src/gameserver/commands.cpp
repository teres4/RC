#include "commands.hpp"

void CommandManager::addCommand(std::shared_ptr<CommandHandler> command)
{
    if (command->_isTCP){
        _handlersTCP.insert({command->_name, command});
        return;
    }
    _handlersUDP.insert({command->_name, command});
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