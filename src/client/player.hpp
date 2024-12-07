#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
// #include "commands.hpp"

class Player
{
private:
  int _plid = 0; // The player id
  int _nT = 1;   // The number of trial
  bool onGoing = false;

public:
  void newPlayer(int plid);

  int getPlid();
  int getnT();
  bool activePlayer();

  void finishGame();
  void increaseNT();
};

class Client
{
private:
  std::string _gsip = DEFAULT_HOSTNAME;
  std::string _gsport = DEFAULT_PORT;
  char* program_path;

public:
  Player _player;

  Client(int argc, char **argv);

  void writeFile(std::string fName, std::stringstream &content);

  void processRequest(ProtocolCommunication &comm);

  // void printHelp(std::ostream& stream);
};

#endif