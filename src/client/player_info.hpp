
#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <cstddef>

class Player_Info
{
public:
    int plid; // The username of the user
    bool hasActiveGame = false;
    int num_trials = 0;
    // a map for the players' attempts?

    Player_Info(int plid);
};

#endif