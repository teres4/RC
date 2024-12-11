#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>

/*A directoria GAMES contém um ficheiro por cada jogo em curso contendo o estado do jogo, e ainda as directorias de jogadores, contendo
cada uma delas os resumos de todos os jogos já terminados para um dado jogador.

O estado de um jogo em curso para um dado PLID encontra-se guardado num ficheiro designado GAME (plid).txt

Os jogos já terminados pertencentes a um dado PLID ficam guardados, para cada player, numa directoria cuja designação é PLID sob a
directoria GAMES. O caminho para a localização dos ficheiros de jogos finalizados é assim: GAMES/PLID.

GAME (plid).txt:
PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s*/

class DatabaseManager
{
private:
    std::string m_rootDir; // Root directory for all data files

    // Helper methods
    bool openFile(std::fstream &fileStream, const std::string &filePath, std::ios_base::openmode mode);
    bool closeFile(std::fstream &fileStream);

public:
    DatabaseManager(const std::string &rootDir);
    ~DatabaseManager();

    bool CreateDirectory(std::string path);
    bool CreateFile(std::string path);
    bool AppendToFile(std::string path, std::string content);
    bool WriteToFile(std::string path, std::string content);
};

class GamedataManager : public DatabaseManager
{

public:
    GamedataManager();

    bool hasOngoingGame(std::string PLID);

    void CreateGame(std::string PLID, char mode, int time, tm startdate, int timestart);

    int FindTopScores();

    int FindLastGame(std::string PLID, std::string fname);
};

#endif