#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include <vector>

#include "player_info.hpp"

class CommandHandler
{

public:
  const char *name;                        // The name of the command
  const std::optional<const char *> alias; // The aliases of the command
  const std::optional<const char *> args;  // The args of the command
  const std::optional<const char *> usage; // The description of the command

  /**
   * @brief Handles the command with the given arguments.
   * should be implemented by derived classes
   */
  virtual void handle(std::string args, Player_Info &receiver) = 0;

  // void readCommand(std::string command, Player_Info &state);

protected:
  /**
   * @brief Constructs a CommandHandler object with the specified properties.
   *
   * @param name The name of the command.
   * @param alias The aliases of the command
   * @param args The args of the command.
   * @param usage The description of the command.
   */

  CommandHandler(const char *__name, const std::optional<const char *> __alias,
                 const std::optional<const char *> __args, const char *__usage)
      : name{__name}, alias{__alias}, args{__args}, usage{__usage} {}
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
  {
  }
};

class DebugCommand : public CommandHandler
{
  void handle(std::string args, Player_Info &state);

public:
  DebugCommand()
      : CommandHandler("debug", std::nullopt, "PLID max_playtime C1 C2 C3 C4",
                       "Exit application") {}
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
      : std::runtime_error("ERROR: " + reason) {};
};

/**
 * @brief Exception thrown when invalid arguments are provided for a command.
 */
class CommandArgumentException : public CommandException
{
public:
  /**
   * @brief Constructs a CommandArgumentException with the specified usage
   * information.
   *
   * @param usage The usage information for the command.
   */
  CommandArgumentException(std::string usage)
      : CommandException("Invalid arguments.\nUsage: " + usage) {};
};

/**
 * @brief Exception thrown when an unknown command is encountered.
 */
class UnknownCommandException : public CommandException
{
public:
  UnknownCommandException() : CommandException("Unknown Command.") {};
};

#endif
