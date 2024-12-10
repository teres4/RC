#ifndef SCOMMANDS_H
#define SCOMMANDS_H

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>

#include "../common/utils.hpp"
#include "../common/protocol.hpp"


class CommandHandler
{
public:
    std::string _name; // The name of the command
    bool _isTCP;       // Specifies whether command is tcp or udp

    /**
     * @brief Handles the command with the given arguments.
     * should be implemented by derived classes
     */
    virtual void handle(MessageSource &message, std::string &response) = 0;

protected:
    /**
     * @brief Constructs a CommandHandler object with the specified properties.
     *
     * @param name The name of the command.
     */

    CommandHandler(const std::string &name, bool isTCP)
        : _name{name},
          _isTCP{isTCP} {}
};

class CommandManager
{
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlers;

public:
    /**
     * @brief Adds a command to the command manager.
     *
     * @param handler The command handler to add.
     */
    void registerCommand(std::shared_ptr<CommandHandler> handle);

    void registerAllCommands();

    std::string handleCommand(std::string message);

};

class StartCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    StartCommand() : CommandHandler("LIN", false) {}
};

class TryCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    TryCommand() : CommandHandler("TRY", false) {}
};

class ShowTrialsCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    ShowTrialsCommand() : CommandHandler("STR", true) {}
};

class ScoreboardCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    ScoreboardCommand() : CommandHandler("SSB", true) {}
};

class QuitCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    QuitCommand() : CommandHandler("QUT", false) {}
};

class ExitCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    ExitCommand() : CommandHandler("QUT", false) {}
};

class DebugCommand : public CommandHandler
{
    void handle(MessageSource &message, std::string &response);

public:
    DebugCommand() : CommandHandler("DBG", false) {}
};

std::vector<std::string> split_command(std::string input);

#endif
