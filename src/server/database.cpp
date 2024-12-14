#include "database.hpp"
#include <filesystem>

#include <ctime>

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
            throw std::runtime_error("Error: Unable to create file");
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

void DatabaseManager::deleteFile(std::string path)
{
    if (std::remove(path.c_str()) != 0)
    { // Try to delete the file
        throw std::runtime_error("Error: Unable to delete file '" + path + "'.");
    }
}

void DatabaseManager::appendToFile(std::string path, std::string content)
{
    try
    {
        std::ofstream fstream;
        fstream.open(path, std::ios_base::app);
        fstream << content;
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Couldn't append to file: " + path + " with content: " + content);
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

void GamedataManager::quitGame(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    deleteFile(path);
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

    return key;
}

std::string GamedataManager::formatSecretKey(std::string key)
{
    std::string result;

    for (size_t i = 0; i < key.length(); ++i)
    {
        result += key[i];
        if (i != key.length() - 1)
        { // Add space if not the last character
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

int GamedataManager::expectedNT(std::string PLID)
{
    std::string path = GAMES_DIR + gameFileName(PLID);
    std::string line;

    std::fstream fileStream;
    fileStream.open(path, std::ios::in);

    if (!fileStream.is_open())
    {
        throw std::runtime_error("Error opening file");
    }
    int i = 0;
    std::getline(fileStream, line); // ignore first line

    while (std::getline(fileStream, line))
    {
        i++;
    }
    i++; // increment to get the expected number of trials

    return i;
}

long int GamedataManager::getOngoingGameTime(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string line;

    std::fstream fileStream;
    fileStream.open(path, std::ios::in);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Error opening file");
    }

    std::getline(fileStream, line);
    std::string word = getiword(line, 7); // get the time()
    std::cout << "time: " << word << std::endl;

    return std::stol(word);
}

long int GamedataManager::timeSinceStart(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    long int starttime = getOngoingGameTime(plid);

    time_t now = time(NULL);

    return (long int)now - starttime;
}

void GamedataManager::registerTry(std::string plid, std::string key, int B, int W)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string content = "T: " + key + " " + std::to_string(B) + " " + std::to_string(W) + " " + std::to_string(timeSinceStart(plid)) + "\n";
    appendToFile(path, content);
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