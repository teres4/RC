#include "database.hpp"
#include <filesystem>

bool DatabaseManager::openFile(std::fstream &fileStream,
                               const std::string &filePath, std::ios_base::openmode mode)
{
    fileStream.open(filePath, mode);
    if (!fileStream.is_open())
    {
        return false;
    }
    return true;
}

bool DatabaseManager::closeFile(std::fstream &fileStream)
{
    fileStream.close();
    return true;
}


void DatabaseManager::createFile(std::string path) {
    // Extract the directory portion from the provided file path.
    std::string directory = std::filesystem::path(path).parent_path().string();

    if (mkdir(directory.c_str(), 0777) == -1) { // Check if the directory portion is not empty.
        if (errno != EEXIST) {
            throw ProtocolException();
        }
    }
}

void DatabaseManager::writeToFile(std::string path, std::string content)
{
    try {
    createFile(path);  // Assure that the directory exists

    std::ofstream file(path);  // Create a file with the given name

    ssize_t n = (ssize_t) content.length();
    file.write(content.c_str(), n);

    file.close();  // Close the file
    }
    catch (...) {
        throw ProtocolException();
    }
}


bool GamedataManager::hasOngoingGame(std::string plid)
{
    if (!validate_plid(plid)){
        throw ProtocolException();
        return false;
    }
    // ongoing games are stored in the GAMES directory
    std::string path = _m_rootDir + gameFileName(plid);
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
    _m_rootDir = rootDir;
}



void GamedataManager::createGame(std::string plid, char mode, int duration, 
                                    std::string dateTime, time_t time)
{
    std::string path = _m_rootDir + gameFileName(plid);
    
    std::string code = generateRandomKey();

    std::string content = plid + " " + mode + " " + code + " " + 
                std::to_string(duration) + " " + dateTime + " " +
                std::to_string(time) + "\n";


    writeToFile(path, content);
}


void GamedataManager::createGame(std::string plid, char mode, std::string key, int duration, 
                        std::string dateTime, time_t time)
{
    std::string path = _m_rootDir + gameFileName(plid);
    
    std::string content = plid + " " + mode + " " + key + " " + 
                std::to_string(duration) + " " + dateTime + " " +
                std::to_string(time) + "\n";


    writeToFile(path, content);
}



// std::string GamedataManager::hourtoString(tm time)
// {
//     std::string timeString = std::to_string(time.tm_hour) + ":" + 
//                     std::to_string(time.tm_min) + ":" + 
//                     std::to_string(time.tm_sec);

//     std::cout << "time string: " << timeString << std::endl;
//     return timeString;
// }

// std::string GamedataManager::dateToString(tm date)
// {
//     std::string dateString = std::to_string(date.tm_year) + "-" + 
//                     std::to_string(date.tm_mon) + "-" + 
//                     std::to_string(date.tm_mday);

//     std::cout << "date string: " << dateString << std::endl;
//     return dateString;
// }