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

/*A directoria GAMES contém um ficheiro por cada jogo em curso contendo o estado do jogo, e ainda as directorias de jogadores, contendo
cada uma delas os resumos de todos os jogos já terminados para um dado jogador.

O estado de um jogo em curso para um dado PLID encontra-se guardado num ficheiro designado GAME (plid).txt

Os jogos já terminados pertencentes a um dado PLID ficam guardados, para cada player, numa directoria cuja designação é PLID sob a
directoria GAMES. O caminho para a localização dos ficheiros de jogos finalizados é assim: GAMES/PLID.

GAME (plid).txt:
PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s*/

class DatabaseManager
{
protected:
    std::string _m_rootDir; // Root directory for all data files

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

private:
    std::string _games_dir = "GAMES/";
    std::string _scores_dir = "PLAYERS/";

public:
    GamedataManager(const std::string rootDir);

    bool hasOngoingGame(std::string plid);
    bool hasGames(std::string plid);
    bool gameShouldEnd(std::string plid);

    void createGame(std::string plid, char mode, int duration,
                    std::string dateTime, time_t time);

    void createGame(std::string plid, char mode, std::string key, int duration,
                    std::string dateTime, time_t time);

    void quitGame(std::string plid);
    void quitAllGames();

    // int findTopScores();

    // int findLastGame(std::string plid, std::string fname);

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

    void formatScoreboard(SCORELIST *list, std::string &fName, int &fSize, std::string &fdata);
    int remainingTime(std::string plid);
    std::string durationOfGame(std::string str);

    void getCurrentGameData(std::string plid, std::string &fName, 
                            int &fSize, std::string &fdata);

    void getMostRecentGameData(std::string plid, std::string &fName, 
                            int &fSize, std::string &fdata);

};

#endif