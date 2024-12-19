#include "database.hpp"
#include <filesystem>
#include <regex>

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

void DatabaseManager::createDir(std::string path)
{
    try
    {
        // Check if the directory already exists
        if (!std::filesystem::exists(path))
            // Create the directory
            std::filesystem::create_directory(path);
    }
    catch (std::exception &e)
    {
        throw UnrecoverableError("Unable to create directory: " + path);
    }
}

void DatabaseManager::createFile(std::string path)
{
    // Extract the directory portion from the provided file path.
    std::string directory = std::filesystem::path(path).parent_path().string();

    if (mkdir(directory.c_str(), 0777) == -1)
    { // Check if the directory portion is not empty.
        if (errno != EEXIST)
        {
            throw UnrecoverableError("Unable to create file: " + path);
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
        ssize_t total_written = 0;

        while (total_written < n)
        {
            file.write(content.c_str() + total_written, n - total_written);

            total_written += (ssize_t)file.tellp() - total_written;
        }

        file.close(); // Close the file
    }
    catch (...)
    {
        throw UnrecoverableError("Couldn't write file: " + path + " with content: " + content);
    }
}

std::string DatabaseManager::renameFile(std::string code)
{
    // yyyy-mm-dd hh-mm-ss
    std::string timedate = currentDateTimeFN();

    std::string filename;

    for (char c : timedate)
    {
        if (c == '-')
            continue;
        else if (c == ' ')
            filename += '_';
        else
            filename += c;
    }
    filename += '_' + code + ".txt";
    return filename;
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
        throw UnrecoverableError("Couldn't append to file: " + path + " with content: " + content);
    }
}

int DatabaseManager::countLinesInFile(std::fstream &fileStream)
{
    int lineCount = 0;
    std::string line;

    // Save the current stream position
    std::streampos initialPos = fileStream.tellg();

    // Read each line and count
    while (std::getline(fileStream, line))
    {
        ++lineCount;
    }

    // Reset the stream to where it was before counting lines
    fileStream.clear(); // Clear EOF flag
    fileStream.seekg(initialPos);
    return lineCount;
}

GamedataManager::GamedataManager()
{
    createDir(FILES_DIR);
    createDir(GAMES_DIR);
    createDir(SCORES_DIR);
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
        fileStream.close();
        return true;
    }

    catch (...)
    {
        return false;
    }
}

bool GamedataManager::hasGames(std::string plid)
{
    try
    {
        validate_plid(plid);
        // ongoing games are stored in the GAMES directory
        std::string path = GAMES_DIR + plid;

        return std::filesystem::is_directory(path);
    }

    catch (...)
    {
        return false;
    }
}

bool GamedataManager::gameShouldEnd(std::string plid)
{
    if (remainingTime(plid) <= 0)
        return true;
    return false;
}

void GamedataManager::gameOver(std::string plid, std::string code)
{
    // create directory with plid and move file
    // delete file from gamedata/games

    std::string src_path = GAMES_DIR + gameFileName(plid);
    std::string dest_path = GAMES_DIR + playerDirectory(plid);

    std::string lastLine = currentDateTime() + " " + std::to_string(timeSinceStart(plid));
    appendToFile(src_path, lastLine);

    std::string newFilename = renameFile(code);

    try
    {
        createDir(dest_path);
        // Construct the destination path (folder + file name)
        std::filesystem::path destPath = std::filesystem::path(dest_path) / newFilename;

        // Move the file to the folder
        std::filesystem::rename(src_path, destPath);
    }
    catch (std::exception &e)
    {
        throw UnrecoverableError("Error moving file: " + src_path + " to " + dest_path);
    }
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
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
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
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
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
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
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
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
    }

    std::getline(fileStream, line);
    std::string word = getiword(line, 7); // get the time()

    return std::stol(word);
}

long int GamedataManager::getOngoingGameTimeLimit(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string line;

    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
    }

    std::getline(fileStream, line);
    std::string word = getiword(line, 4); // get the time()

    return std::stol(word);
}

std::string GamedataManager::ongoingGameMode(std::string plid)
{
    std::string path = GAMES_DIR + gameFileName(plid);
    std::string line;

    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::in))
    {
        throw UnrecoverableError("Error opening file: " + path);
    }

    std::getline(fileStream, line);
    std::string word = getiword(line, 2);

    if (word == "P")
    {
        return "PLAY";
    }
    else
    {
        return "DEBUG";
    }

    return word;
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
    std::string content = "T: " + key + " " + std::to_string(B) + " " +
                          std::to_string(W) + " " + std::to_string(timeSinceStart(plid)) + "\n";
    appendToFile(path, content);
}

void GamedataManager::makeScoreFile(std::string plid)
{
    // score PLID DDMMYYYY HHMMSS.txt
    // format: SSS, add zeros to the left
    int nT = expectedNT(plid) - 1;
    std::string score = std::to_string(rand() % 20 + (8 - nT) * 10); // better score with lower nT
    while (score.length() < 3)
    {
        score = "0" + score;
    }
    std::string timedate = currentDateTimeFN();
    std::string filename = score + "_" + plid + "_" + timedate + ".txt";

    std::string path = SCORES_DIR + filename;
    std::string content = score + " " + plid + " " + getsecretKey(plid) + " " + std::to_string(nT) + " " + ongoingGameMode(plid) + " " + "\n";

    writeToFile(path, content);
}

void GamedataManager::gameWon(std::string plid)
{
    makeScoreFile(plid);
    gameOver(plid, WIN_CODE);
}

void GamedataManager::gameLost(std::string plid)
{
    gameOver(plid, FAIL_CODE);
}

void GamedataManager::gameTimeout(std::string plid)
{
    gameOver(plid, TIMEOUT_CODE);
}

void GamedataManager::quitGame(std::string plid)
{
    gameOver(plid, QUIT_CODE);
}

void GamedataManager::quitAllGames()
{
    std::string directory = GAMES_DIR;

    std::regex pattern("^GAME_.*\\.txt$");

    for (auto entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file())
        { // Check if it's a file
            std::string fileName = entry.path().filename().string();

            // Match the filename using the regular expression
            if (std::regex_match(fileName, pattern))
            {
                std::string plid = fileName.substr(5, 6);
                quitGame(plid);
            }
        }
    }
}

int GamedataManager::remainingTime(std::string plid)
{
    long int time = getOngoingGameTimeLimit(plid);
    long int now = timeSinceStart(plid);
    return (int)(time - now);
}

std::string GamedataManager::durationOfGame(std::string lastLineOfFile)
{
    return getiword(lastLineOfFile, 3);
}

void GamedataManager::formatScoreboard(SCORELIST *list, std::string &fName,
                                       int &fSize, std::string &fdata, int nscores)
{
    fName = "TOPSCORE_" + truncateDate(currentDateTimeFN()) + ".txt";

    char buffer[BUFFER_SIZE];
    char mode[6];
    int i = 0;

    sprintf(buffer, "\n\t\t\t------ TOP %d SCORES ------\n\n", nscores);
    fdata += buffer;

    sprintf(buffer, "\t\tSCORE \tPLAYER \tCODE\tNO TRIALS  MODE\n\n");
    fdata += buffer;
    while (i < 10 && list->score[i] != 0 && i < nscores)
    {
        if (list->mode[i] == MODEDEBUG)
        {
            strcpy(mode, "DEBUG");
        }
        else
            strcpy(mode, "PLAY");

        sprintf(buffer, "\t%d ------ %d\t%s\t%s\t%d\t%s\n", i + 1,
                list->score[i], list->PLID[i], list->color_code[i],
                list->ntries[i], mode);
        fdata += buffer;

        i++;
    }
    fSize = (int)fdata.length();

    fdata += "\n";
}

void GamedataManager::getCurrentGameData(std::string plid,
                                         std::string &fName, int &fSize, std::string &fdata)
{
    std::string path = GAMES_DIR + gameFileName(plid);

    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::in))
        throw UnrecoverableError("Error opening file");

    fName = gameFileName(plid);

    int number_trials = countLinesInFile(fileStream) - 1;

    std::string line;
    std::getline(fileStream, line);

    std::string game_duration = getiword(line, 4);
    std::string dateTime = getiword(line, 5) + ' ' + getiword(line, 6);

    fdata = "\n\tActive game found for player " + plid + '\n';
    fdata += "Game initiated: " + dateTime + " with " + game_duration +
             " seconds to be completed\n";

    fdata += "\n\t--- Transactions found: " + std::to_string(number_trials) + " ---\n\n";

    while (number_trials > 0)
    {
        std::getline(fileStream, line);
        std::string trial = getiword(line, 2);
        std::string nB = getiword(line, 3);
        std::string nW = getiword(line, 4);
        std::string time = getiword(line, 5);

        fdata += "Trial: " + trial + ", nB: " + nB + ", nW: " + nW +
                 " at " + time + "s\n";

        number_trials--;
    }

    fdata += "\n\t-- " + std::to_string(remainingTime(plid)) + " seconds remaining to be completed --\n";

    fSize = (int)fdata.length();

    fdata += "\n";
}

void GamedataManager::getMostRecentGameData(std::string plid, std::string &fName,
                                            int &fSize, std::string &fdata)
{

    std::string path;
    findLastGame(plid, path);

    std::fstream fileStream;
    if (!openFile(fileStream, path, std::ios::in))
        throw UnrecoverableError("Error opening file");

    fName = gameFileName(plid);

    int number_trials = countLinesInFile(fileStream) - 2;

    std::string line;
    std::getline(fileStream, line);

    std::string mode = getiword(line, 2);
    std::string key = getiword(line, 3);
    std::string game_duration = getiword(line, 4);
    std::string dateTime = getiword(line, 5) + ' ' + getiword(line, 6);

    if (mode == "P")
        mode = "PLAY";
    else
        mode = "DEBUG";

    fdata = "\n\tLast finalized game for player " + plid + '\n';
    fdata += "Game initiated: " + dateTime + " with " + game_duration +
             " seconds to be completed\n";
    fdata += "Mode: " + mode + "\tSecret code: " + key + "\n";
    fdata += "\n\t--- Transactions found: " + std::to_string(number_trials) + " ---\n\n";

    while (number_trials > 0)
    {
        std::getline(fileStream, line);
        std::string trial = getiword(line, 2);
        std::string nB = getiword(line, 3);
        std::string nW = getiword(line, 4);
        std::string time = getiword(line, 5);

        fdata += "Trial: " + trial + ", nB: " + nB + ", nW: " + nW +
                 " at " + time + "s\n";

        number_trials--;
    }
    std::getline(fileStream, line);
    fdata += "\n\t-- " + durationOfGame(line) + " seconds of play time --\n";

    fSize = (int)fdata.length();

    fdata += "\n";
}
