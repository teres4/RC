#include "database.hpp"

bool DatabaseManager::openFile(std::fstream &fileStream,
                               const std::string &filePath, std::ios_base::openmode mode)
{
    fileStream.open(filePath, mode);
    if (!fileStream.is_open())
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::closeFile(std::fstream &fileStream)
{
    fileStream.close();
    return true;
}

bool DatabaseManager::createDirectory(std::string path)
{
    if (mkdir(path.c_str(), 0777) == -1)
    {
        std::cerr << "Error creating directory: " << path << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::createFile(std::string path)
{
    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::out))
    {
        return false;
    }
    closeFile(fileStream);
    return true;
}

bool DatabaseManager::appendToFile(std::string path, std::string content)
{
    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::app))
    {
        return false;
    }
    fileStream << content;
    closeFile(fileStream);
    return true;
}

bool GamedataManager::hasOngoingGame(std::string PLID)
{

    // ongoing games are stored in the GAMES directory
    std::string path = m_rootDir + gameFileName(PLID);
    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::in))
    {
        return false;
    }
    closeFile(fileStream);
    return true;
}

GamedataManager::GamedataManager(const std::string rootDir)
{
    m_rootDir = rootDir;
}

void GamedataManager::createGame(std::string PLID, char mode, int time, tm startdate, int timestart)
{
    std::string path = m_rootDir + gameFileName(PLID);

    std::string code = generateRandomKey();
    std::string content = PLID + " " + mode + " " + code + " " + std::to_string(time) + " " + dateToString(startdate) + " " + HourtoString(startdate) + std::to_string(timestart) + "\n";
    createFile(path);
    appendToFile(path, content);
}

std::string GamedataManager::HourtoString(tm time)
{
    std::string timeString = std::to_string(time.tm_hour) + ":" + std::to_string(time.tm_min) + ":" + std::to_string(time.tm_sec);
    return timeString;
}

std::string GamedataManager::dateToString(tm date)
{
    std::string dateString = std::to_string(date.tm_year) + "-" + std::to_string(date.tm_mon) + "-" + std::to_string(date.tm_mday);
    return dateString;
}