#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "commands.hpp"
#include "common/constants.hpp"
#include "common/utils.hpp"


class Client {
 public:
  // char* program_path;
  std::string host = DEFAULT_HOSTNAME;
  std::string port = DEFAULT_PORT;
  // bool help = false;

  Client(int argc, char* argv[]);
  // void printHelp(std::ostream& stream);
};

// void registerCommands(CommandManager& manager);

#endif