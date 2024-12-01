#ifndef PLAYER_H
#define PLAYER_H

#include <csignal>

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../common/protocol.hpp"
// #include "commands.hpp"


class Player{
  private:
    int _plid = 0;  // The player id
    int _nT = 1;    // The number of trial
    bool onGoing = false;


  public:
    void newPlayer(int plid);

    int getPlid();
    int getnT();
    bool activePlayer();

    void increaseNT();

    

};

// class Game {
//  protected:
//   uint32_t numErrors = 0;
//   uint32_t currentTrial = 1;
//   bool onGoing = true;
//   uint32_t playerId;
//   uint32_t wordLen;
//   uint32_t maxErrors;

//  public:
//   uint32_t getPlayerId();
//   uint32_t getWordLen();
//   uint32_t getMaxErrors();
//   uint32_t getCurrentTrial();
//   uint32_t getGoodTrials();
//   uint32_t getNumErrors();
//   bool isOnGoing();
//   void finishGame();  // setOnGoing(false)
// };

class Client
{
  private:
    std::string _gsip = DEFAULT_HOSTNAME;
    std::string _gsport = DEFAULT_PORT;
    // char* program_path;
  
  public:
    Player _player;

    Client(int argc, char **argv);

    void processRequest(ProtocolCommunication &comm);


    // void printHelp(std::ostream& stream);
};




#endif