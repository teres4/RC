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

void DatabaseManager::createFile(std::string path)
{
    // Extract the directory portion from the provided file path.
    std::string directory = std::filesystem::path(path).parent_path().string();

    if (mkdir(directory.c_str(), 0777) == -1)
    { // Check if the directory portion is not empty.
        if (errno != EEXIST)
        {
            throw std::runtime_error("Error creating file");
        }
    }
}

void DatabaseManager::writeToFile(std::string path, std::string content)
{
    try
    {
        createFile(path); // Assure that the directory exists

        std::ofstream file(path); // Create a file with the given name

        ssize_t n = (ssize_t)content.length();
        file.write(content.c_str(), n);

        file.close(); // Close the file
    }
    catch (...)
    {
        throw std::runtime_error("Couldn't write file: " + path + " with content: " + content);
    }
}

bool GamedataManager::hasOngoingGame(std::string plid)
{
    try
    {
        validate_plid(plid);
        // ongoing games are stored in the GAMES directory
        std::string path = GAMES_DIR + gameFileName(plid);
        std::fstream fileStream;
        if (!openFile(fileStream, path, std::ios::in))
        {
            return false;
        }
        closeFile(fileStream);
        return true;
    }

    catch (...)
    {
        return false;
    }
}

GamedataManager::GamedataManager(const std::string rootDir)
{
    _m_rootDir = rootDir;
}

void GamedataManager::createGame(std::string plid, char mode, int duration,
                                 std::string dateTime, time_t time)
{
    validate_plid(plid);
    validate_playTime(duration);

    std::string path = GAMES_DIR + gameFileName(plid);

    std::string code = generateRandomKey();

    std::string content = plid + " " + mode + " " + code + " " +
                          std::to_string(duration) + " " + dateTime + " " +
                          std::to_string(time) + "\n";

    writeToFile(path, content);
}

void GamedataManager::createGame(std::string plid, char mode, std::string key, int duration,
                                 std::string dateTime, time_t time)
{
    validate_plid(plid);
    validate_playTime(duration);

    std::string path = GAMES_DIR + gameFileName(plid);

    std::string content = plid + " " + mode + " " + key + " " +
                          std::to_string(duration) + " " + dateTime + " " +
                          std::to_string(time) + "\n";

    writeToFile(path, content);
}

std::string GamedataManager::getsecretKey(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string key;

    // extract 3rd word from the file

    std::fstream fileStream;
    fileStream.open(path, std::ios::in);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Error opening file");
    }

    std::string line;
    std::getline(fileStream, line);

    key = getiword(line, 3);


    return formatSecretKey(key);
}


std::string GamedataManager::formatSecretKey(std::string key){
    std::string result;

    for (size_t i = 0; i < key.length(); ++i) {
        result += key[i];
        if (i != key.length() - 1) { // Add space if not the last character
            result += ' ';
        }
    }

    return result;

}


std::string GamedataManager::getiword(std::string line, int n)
{
    std::istringstream iss(line);
    std::string word;
    for (int i = 0; i < n; i++)
    {
        iss >> word;
    }

    return word;
}

bool GamedataManager::isRepeatedTrial(std::string plid, std::string key)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string line;

    std::fstream fileStream;
    fileStream.open(path, std::ios::in);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Error opening file");
    }

    std::getline(fileStream, line); // ignore first line
    while (std::getline(fileStream, line))
    {
        std::string word = getiword(line, 2); // get second word
        if (word == key)
        {
            return true;
        }
    }

    return false;
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