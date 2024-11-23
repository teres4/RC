#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>

#include "player_info.hpp"
#include "../common/utils.hpp"


class CommandHandler
{
  public:
    std::string name;                        // The name of the command
    std::optional<std::string> alias; // The aliases of the command
    std::optional<std::string> command_arg;  // The args of the command
    std::optional<std::string> description; // The description of the command

    /**
     * @brief Handles the command with the given arguments.
     * should be implemented by derived classes
     */
    virtual void handle(std::string args, Player_Info &receiver) = 0;


  protected:
    /**
     * @brief Constructs a CommandHandler object with the specified properties.
     *
     * @param name The name of the command.
     * @param alias The aliases of the command
     * @param args The args of the command.
     * @param usage The description of the command.
     */


    CommandHandler(const std::string& __name, 
                  const std::optional<std::string>& __alias,
                  const std::optional<std::string>& __command_arg, 
                  const std::string& __description)
        : name{__name}, 
          alias{__alias}, 
          command_arg{__command_arg}, 
          description{__description} {}
};


class CommandManager
{
  // list of command handlers
  std::vector<std::shared_ptr<CommandHandler>> handlerList;
  // name->handler map
  std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers;

  public:
    // void printHelp();

    /**
     * @brief Adds a command to the command manager.
     *
     * @param handler The command handler to add.
     */
    void addCommand(std::shared_ptr<CommandHandler> handler);

    /**
     * @brief Waits for a command from the player and processes it.
     *
     * @param state A reference to a Player_Info structure that will be 
     * updated based on the received command.
     */
    void waitForCommand(Player_Info &state);
};


class StartCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    StartCommand()
        : CommandHandler("start", std::nullopt, "PLID max_playtime",
                        "Start a new game") {}
};


class TryCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    TryCommand()
        : CommandHandler("try", std::nullopt, "C1 C2 C3 C4",
                        "Tries a combination of colors") {}
};

class ShowTrialsCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    ShowTrialsCommand()
        : CommandHandler(
              "show_trials", "st", std::nullopt,
              "Display previously made trials and respective results") {}
};

class ScoreboardCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    ScoreboardCommand()
        : CommandHandler("scoreboard", "sb", std::nullopt,
                        "Display the scoreboard") {}
};

class QuitCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    QuitCommand()
        : CommandHandler("quit", std::nullopt, std::nullopt, "Quit game") {}
};

class ExitCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    ExitCommand()
        : CommandHandler("exit", std::nullopt, std::nullopt, "Exit application")
    {}
};

class DebugCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

  public:
    DebugCommand()
        : CommandHandler("debug", std::nullopt, "PLID max_playtime C1 C2 C3 C4",
                        "Start a game in debug mode") {}
};



/**
 * @brief Exception class for command-related errors.
 *
 * This class is derived from std::runtime_error and is used to represent
 * exceptions that occur during command execution.
 */
class CommandException : public std::runtime_error
{
  public:
    /**
     * @brief Constructs a CommandException object with the specified reason.
     *
     * @param reason The reason for the exception.
     */
    CommandException(std::string reason)
        : std::runtime_error("ERROR: " + reason + "\n") {};
};


/**
 * @brief Exception thrown when invalid arguments are provided for a command.
 */
class StartCommandArgumentException : public CommandException
{
  public:
    /**
     * @brief Constructs a CommandArgumentException with the specified command
     * args information.
     *
     * @param command_arg The arguments needed for the command.
     */
    StartCommandArgumentException(std::string &command_arg)
        : CommandException("Invalid arguments.\nUsage: " + command_arg + 
          "\nPLID must be a 6-digit number. max_playtime cannot exceed 600 seconds")
          {};
};

/**
 * @brief Exception thrown when an unknown command is encountered.
 */
class UnknownCommandException : public CommandException
{
  public:
    UnknownCommandException() : CommandException("Unknown Command.\n") {};
};


std::vector<std::string> split_command(std::string input);


#endif
