#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>

#include "../common/utils.hpp"
#include "../common/protocol.hpp"

class DatabaseManager
{
    // Helper methods

public:
    bool openFile(std::fstream &fileStream, const std::string &filePath, std::ios_base::openmode mode);
    bool closeFile(std::fstream &fileStream);
    void deleteFile(std::string path);
    void createDir(std::string path);
    void createFile(std::string path);
    void appendToFile(std::string path, std::string content);
    void writeToFile(std::string path, std::string content);
    std::string renameFile(std::string path);
    int countLinesInFile(std::fstream &fileStream);
};

class GamedataManager : public DatabaseManager
{

public:
    GamedataManager();

    bool hasOngoingGame(std::string plid);
    bool hasGames(std::string plid);
    bool gameShouldEnd(std::string plid);

    void createGame(std::string plid, char mode, int duration,
                    std::string dateTime, time_t time);

    void createGame(std::string plid, char mode, std::string key, int duration,
                    std::string dateTime, time_t time);

    void quitGame(std::string plid);
    void quitAllGames();

    void gameOver(std::string plid, std::string code);

    std::string getsecretKey(std::string plid);
    std::string formatSecretKey(std::string key);
    bool isRepeatedTrial(std::string plid, std::string key);
    int expectedNT(std::string PLID);
    std::string ongoingGameMode(std::string plid);

    void registerTry(std::string plid, std::string key, int B, int W);

    long int getOngoingGameTime(std::string plid);
    long int getOngoingGameTimeLimit(std::string plid);
    long int timeSinceStart(std::string plid);

    std::string getiword(std::string line, int n);

    void gameWon(std::string plid);
    void gameLost(std::string plid);
    void gameTimeout(std::string plid);

    void makeScoreFile(std::string plid);

    void formatScoreboard(SCORELIST *list, std::string &fName, int &fSize,
                          std::string &fdata, int nscores);
    int remainingTime(std::string plid);
    std::string durationOfGame(std::string str);

    void getCurrentGameData(std::string plid, std::string &fName,
                            int &fSize, std::string &fdata);

    void getMostRecentGameData(std::string plid, std::string &fName,
                               int &fSize, std::string &fdata);
};

#endif