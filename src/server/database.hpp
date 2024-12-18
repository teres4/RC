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

class DatabaseManager
{
    // Helper methods

public:
    /**
     * @brief Opens a file with the given mode.
     *
     * @param fileStream Reference to the file stream object.
     * @param filePath Path to the file to be opened.
     * @param mode Mode in which the file should be opened.
     * @return true if the file was successfully opened, false otherwise.
     */
    bool openFile(std::fstream &fileStream, const std::string &filePath, std::ios_base::openmode mode);

    /**
     * @brief Deletes the file at the specified path.
     *
     * @param path Path to the file to be deleted.
     */
    void deleteFile(std::string path);

    /**
     * @brief Creates a directory at the specified path.
     *
     * @param path Path where the directory should be created.
     */
    void createDir(std::string path);

    /**
     * @brief Creates a file at the specified path.
     *
     * @param path Path where the file should be created.
     */
    void createFile(std::string path);

    /**
     * @brief Appends content to the file at the specified path.
     *
     * @param path Path to the file.
     * @param content Content to be appended to the file.
     */
    void appendToFile(std::string path, std::string content);

    /**
     * @brief Writes content to the file at the specified path, overwriting any existing content.
     *
     * @param path Path to the file.
     * @param content Content to be written to the file.
     */
    void writeToFile(std::string path, std::string content);

    /**
     * @brief Renames the file at the specified path.
     *
     * @param path Path to the file to be renamed.
     * @return std::string New path of the renamed file.
     */
    std::string renameFile(std::string path);

    /**
     * @brief Counts the number of lines in the given file stream.
     *
     * @param fileStream Reference to the file stream object.
     * @return int Number of lines in the file.
     */
    int countLinesInFile(std::fstream &fileStream);
};

class GamedataManager : public DatabaseManager
{
public:
    /**
     * @class GamedataManager
     * @brief Manages game data for players.
     *
     * This class provides methods to manage game data, including creating games,
     * checking game status, registering tries, and handling game outcomes.
     */

    /**
     * @brief Constructor for GamedataManager.
     */
    GamedataManager();

    /**
     * @brief Checks if a player has a Game file for an ongoing game.
     * @param plid Player ID.
     * @return True if the player has an ongoing game, false otherwise.
     */
    bool hasOngoingGame(std::string plid);

    /**
     * @brief Checks if a player has any games (If it has a directory in GAMES).
     * @param plid Player ID.
     * @return True if the player has games, false otherwise.
     */
    bool hasGames(std::string plid);

    /**
     * @brief Checks if a game has ran out of time.
     * @param plid Player ID.
     * @return True if the game should end, false otherwise.
     */
    bool gameShouldEnd(std::string plid);

    /**
     * @brief Creates a file for a new game.
     * @param plid Player ID.
     * @param mode Game mode.
     * @param duration Game duration.
     * @param dateTime Game start date and time.
     * @param time Game start time.
     */
    void createGame(std::string plid, char mode, int duration, std::string dateTime, time_t time);

    /**
     * @brief Createsa file for a new game, with a given secret key.
     * @param plid Player ID.
     * @param mode Game mode.
     * @param key Secret key.
     * @param duration Game duration.
     * @param dateTime Game start date and time.
     * @param time Game start time.
     */
    void createGame(std::string plid, char mode, std::string key, int duration, std::string dateTime, time_t time);

    /**
     * @brief Quits the ongoing game for a player.
     * @param plid Player ID.
     */
    void quitGame(std::string plid);

    /**
     * @brief Quits all ongoing games.
     */
    void quitAllGames();

    /**
     * @brief Ends the game for a player with a specific code.
     * @param plid Player ID.
     * @param code End game code.
     */
    void gameOver(std::string plid, std::string code);

    /**
     * @brief Gets the secret key for a player's game.
     * @param plid Player ID.
     * @return Secret key.
     */
    std::string getsecretKey(std::string plid);

    /**
     * @brief Adds spaces to a key.
     * @param key Secret key "CCCC".
     * @return Formatted secret key "C C C C".
     */
    std::string formatSecretKey(std::string key);

    /**
     * @brief Checks if a trial is repeated for a players ongoing game.
     * @param plid Player ID.
     * @param key Trial key.
     * @return True if the trial is repeated, false otherwise.
     */
    bool isRepeatedTrial(std::string plid, std::string key);

    /**
     * @brief Gets the expected number of trials for a players ongoing game.
     * @param PLID Player ID.
     * @return Expected number of trials.
     */
    int expectedNT(std::string PLID);

    /**
     * @brief Gets the mode of the ongoing game for a player.
     * @param plid Player ID.
     * @return Ongoing game mode.
     */
    std::string ongoingGameMode(std::string plid);

    /**
     * @brief Registers a try for a player.
     * @param plid Player ID.
     * @param key Trial key.
     * @param B Number of correct positions.
     * @param W Number of correct colors in wrong positions.
     */
    void registerTry(std::string plid, std::string key, int B, int W);

    /**
     * @brief Gets the time of the ongoing game for a player.
     * @param plid Player ID.
     * @return Ongoing game time.
     */
    long int getOngoingGameTime(std::string plid);

    /**
     * @brief Gets the time limit of the ongoing game for a player.
     * @param plid Player ID.
     * @return Ongoing game time limit.
     */
    long int getOngoingGameTimeLimit(std::string plid);

    /**
     * @brief Gets the time since the start of the game for a player.
     * @param plid Player ID.
     * @return Time since start.
     */
    long int timeSinceStart(std::string plid);

    /**
     * @brief Gets the nth word from a line.
     * @param line Input line.
     * @param n Word index.
     * @return nth word.
     */
    std::string getiword(std::string line, int n);

    /**
     * @brief Marks the game as won for a player.
     * @param plid Player ID.
     */
    void gameWon(std::string plid);

    /**
     * @brief Marks the game as lost for a player.
     * @param plid Player ID.
     */
    void gameLost(std::string plid);

    /**
     * @brief Marks the game as timed out for a player.
     * @param plid Player ID.
     */
    void gameTimeout(std::string plid);

    /**
     * @brief Creates a score file from a finished game.
     * @param plid Player ID.
     */
    void makeScoreFile(std::string plid);

    /**
     * @brief Formats the scoreboard.
     * @param list Score list.
     * @param fName File name.
     * @param fSize File size.
     * @param fdata File data.
     * @param nscores Number of scores.
     */
    void formatScoreboard(SCORELIST *list, std::string &fName, int &fSize, std::string &fdata, int nscores);

    /**
     * @brief Gets the remaining time for a player's game.
     * @param plid Player ID.
     * @return Remaining time.
     */
    int remainingTime(std::string plid);

    /**
     * @brief Gets the duration of a game from a string.
     * @param str Input string.
     * @return Game duration.
     */
    std::string durationOfGame(std::string str);

    /**
     * @brief Gets the game data from a players current game.
     * @param plid Player ID.
     * @param fName File name.
     * @param fSize File size.
     * @param fdata File data.
     */
    void getCurrentGameData(std::string plid, std::string &fName, int &fSize, std::string &fdata);

    /**
     * @brief Gets the most recent game data for a player.
     * @param plid Player ID.
     * @param fName File name.
     * @param fSize File size.
     * @param fdata File data.
     */
    void getMostRecentGameData(std::string plid, std::string &fName, int &fSize, std::string &fdata);
};

#endif