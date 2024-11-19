#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "commands.hpp"
#include "common/constants.hpp"
#include "common/utils.hpp"

class Client
{
public:
  // char* program_path;
  std::string host = DEFAULT_HOSTNAME;
  std::string port = DEFAULT_PORT;
  // bool help = false;

  Client(int argc, char *argv[]);
  // void printHelp(std::ostream& stream);
};

// void registerCommands(CommandManager& manager);

class Player_Info
{
public:
  int plid; // The username of the user
  bool hasActiveGame = false;
  int num_trials = 0;
  // a map for the players' attempts?

  Player_Info(int plid);
};

// TODO: implement Game_info class
// put game_info in player info

#endif