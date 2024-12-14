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
    void createFile(std::string path);
    void appendToFile(std::string path, std::string content);
    void writeToFile(std::string path, std::string content);
};

class GamedataManager : public DatabaseManager
{

private:
    std::string _games_dir = "GAMES/";
    std::string _scores_dir = "PLAYERS/";

public:
    GamedataManager(const std::string rootDir);

    bool hasOngoingGame(std::string plid);

    void createGame(std::string plid, char mode, int duration,
                    std::string dateTime, time_t time);

    void createGame(std::string plid, char mode, std::string key, int duration,
                    std::string dateTime, time_t time);

    void quitGame(std::string plid);

    int findTopScores();

    int findLastGame(std::string plid, std::string fname);

    std::string hourtoString(tm time);
    std::string dateToString(tm time);
    std::string getsecretKey(std::string plid);
    std::string formatSecretKey(std::string key);
    bool isRepeatedTrial(std::string plid, std::string key);

    std::string getiword(std::string line, int n);
};

#endif