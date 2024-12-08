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
#include "server.hpp"

class CommandHandler
{
public:
    std::string _name;    // The name of the command
    bool _isTCP;          // Specifies whether command is tcp or udp      

    /**
     * @brief Handles the command with the given arguments.
     * should be implemented by derived classes
     */
    virtual void handle(std::string &message, Server &receiver) = 0;

protected:
    /**
     * @brief Constructs a CommandHandler object with the specified properties.
     *
     * @param name The name of the command.
     */

    CommandHandler(const std::string &name, bool isTCP)
        : _name{name},
        _isTCP {isTCP} {}
};

class CommandManager
{
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlersUDP;
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlersTCP;
    
public:
    /**
     * @brief Adds a command to the command manager.
     *
     * @param handler The command handler to add.
     */
    void addCommand(std::shared_ptr<CommandHandler> handle);

    void addAllCommands();

    /**
     * @brief Waits for a command from the player and processes it.
     *
     * @param state A reference to a Client structure that will be
     * updated based on the received command.
     */
    void waitForCommand(Server &server);
};

class StartCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    StartCommand(): CommandHandler("LIN", false) {}
};

class TryCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    TryCommand(): CommandHandler("TRY", false) {}
};

class ShowTrialsCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    ShowTrialsCommand(): CommandHandler("STR", true) {}
};

class ScoreboardCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    ScoreboardCommand(): CommandHandler("SSB", true) {}
};

class QuitCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    QuitCommand(): CommandHandler("QUT", false) {}
};

class ExitCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    ExitCommand(): CommandHandler("QUT", false) {}
};

class DebugCommand : public CommandHandler
{
    void handle(std::string &args, Server &server);

public:
    DebugCommand(): CommandHandler("DBG", false) {}
};

std::vector<std::string> split_command(std::string input);

#endif
