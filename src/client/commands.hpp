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
#include "player.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"

extern bool exit_command;

class CommandHandler
{
public:
  std::string _name;                       // The name of the command
  std::string _alias;                      // The alias of the command
  std::optional<std::string> _command_arg; // The args of the command
  std::optional<std::string> _description; // The description of the command

  /**
   * @brief Handles the command with the given arguments.
   * should be implemented by derived classes
   */
  virtual void handle(std::string args, Client &receiver) = 0;

protected:
  /**
   * @brief Constructs a CommandHandler object with the specified properties.
   *
   * @param name The name of the command.
   * @param alias The aliases of the command
   * @param args The args of the command.
   * @param usage The description of the command.
   */

  CommandHandler(const std::string &name,
                 const std::string &alias,
                 const std::optional<std::string> &command_arg,
                 const std::string &description)
      : _name{name},
        _alias{alias},
        _command_arg{command_arg},
        _description{description} {}
};

class CommandManager
{
  // list of command handlers
  // std::vector<std::shared_ptr<CommandHandler>> _handlerList;
  
  // name->handler map
  std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlers;

public:
  // void printHelp();

  /**
   * @brief Adds a command to the command manager.
   *
   * @param handler The command handler to add.
   */
  void addCommand(std::shared_ptr<CommandHandler> handler);

  void addAllCommands();

  /**
   * @brief Waits for a command from the player and processes it.
   *
   * @param state A reference to a Client structure that will be
   * updated based on the received command.
   */
  void waitForCommand(Client &state);
};

class StartCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  StartCommand()
      : CommandHandler("start", "", "PLID max_playtime", "Start a new game") {}
};

class TryCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  TryCommand()
      : CommandHandler("try", "", "C1 C2 C3 C4", "Tries a combination of colors") {}
};

class ShowTrialsCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  ShowTrialsCommand()
      : CommandHandler("show_trials", "st", std::nullopt,
            "Display previously made trials and respective results") {}
};

class ScoreboardCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  ScoreboardCommand()
      : CommandHandler("scoreboard", "sb", std::nullopt, "Display the scoreboard") {}
};

class QuitCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  QuitCommand()
      : CommandHandler("quit", "", std::nullopt, "Quit game") {}
};

class ExitCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  ExitCommand()
      : CommandHandler("exit", "", std::nullopt, "Exit application")
  {
  }
};

class DebugCommand : public CommandHandler
{
  void handle(std::string args, Client &state);

public:
  DebugCommand()
      : CommandHandler("debug", "", "PLID max_playtime C1 C2 C3 C4",
                       "Start a game in debug mode") {}
};




std::vector<std::string> split_command(std::string input);

#endif
