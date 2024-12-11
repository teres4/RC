#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>

#include "../common/utils.hpp"

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
    std::string m_rootDir; // Root directory for all data files

    // Helper methods

public:

    bool openFile(std::fstream &fileStream, const std::string &filePath, std::ios_base::openmode mode);
    bool closeFile(std::fstream &fileStream);
    bool createDirectory(std::string path);
    bool createFile(std::string path);
    bool appendToFile(std::string path, std::string content);
    bool writeToFile(std::string path, std::string content);
};


class GamedataManager : public DatabaseManager
{
public:
    GamedataManager(const std::string rootDir);

    bool hasOngoingGame(std::string PLID);

    void createGame(std::string PLID, char mode, int time, tm startdate, int timestart);

    int findTopScores();

    int findLastGame(std::string PLID, std::string fname);
};

#endif