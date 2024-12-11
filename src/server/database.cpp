#include "database.hpp"

DatabaseManager::DatabaseManager(const std::string &rootDir) : m_rootDir(rootDir)
{
}

bool DatabaseManager::openFile(std::fstream &fileStream, const std::string &filePath, std::ios_base::openmode mode)
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

bool DatabaseManager::CreateDirectory(std::string path)
{
    if (mkdir(path.c_str(), 0777) == -1)
    {
        std::cerr << "Error creating directory: " << path << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::CreateFile(std::string path)
{
    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::out))
    {
        return false;
    }
    closeFile(fileStream);
    return true;
}

bool DatabaseManager::AppendToFile(std::string path, std::string content)
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

GamedataManager::GamedataManager(const std::string rootDir) : DatabaseManager(rootDir)
{
}
