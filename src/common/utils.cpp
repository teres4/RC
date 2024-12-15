#include "utils.hpp"
#include "protocol.hpp"

#include <iostream>
#include <unordered_map>
#include <dirent.h>

bool is_exiting = false;

bool is_numeric(std::string &str)
{
    for (char c : str)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }
    return true;
}

bool is_not_numeric(std::string &str)
{
    if (is_numeric(str) == true)
        return false;

    return true;
}

void validate_port(std::string &port)
{
    if (is_numeric(port) == false)
        throw UnrecoverableError("Invalid port: not a number");

    try
    {
        int32_t parsed_port = std::stoi(port);
        if (parsed_port < 1 || parsed_port > 65535)
            throw std::runtime_error("");
    }
    catch (...)
    {
        throw UnrecoverableError(
            "Invalid port: it must be a number between 1 and 65535");
    }
}

/**
 * Check if the plid is valid
 */
void validate_plid(std::string plid)
{
    if (plid.length() != PLID_MAX_SIZE || is_numeric(plid) == false)
    {
        throw ProtocolMessageErrorException();
    }

    int parsed = std::stoi(plid);
    if (parsed < 0)
    {
        throw ProtocolMessageErrorException();
    }
}

int get_plid(std::string plid)
{
    return std::stoi(plid);
}

void validate_playTime(std::string playtime)
{
    if (is_numeric(playtime) == false)
    {
        throw ProtocolMessageErrorException();
    }

    int parsed = std::stoi(playtime);

    if (parsed < 0 || parsed > MAX_PLAYTIME)
    {
        throw ProtocolMessageErrorException();
    }
}

void validate_playTime(int playtime)
{
    if (playtime < 0 || playtime > MAX_PLAYTIME)
    {
        throw ProtocolMessageErrorException();
    }
}

bool validate_color(std::string color)
{
    std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};

    for (auto c : colors)
    {
        // Check if the read string is one of the options
        if (c == color)
        {
            return true;
        }
    }
    return false;
}

void validate_key(std::vector<std::string> colors)
{
    for (auto c : colors)
    {
        if (!validate_color(c))
        {
            throw ProtocolMessageErrorException();
        }
    }
}

void validate_key(std::string key)
{
    for (char c : key)
    {
        if (c == ' ' || c == '\n')
            continue;

        std::string color(1, c);
        if (!validate_color(color))
        {
            throw ProtocolMessageErrorException();
        }
    }
}

std::string get_color_key(std::string args)
{
    // cant end with a space
    if (args[args.length() - 1] == ' ')
    {
        // take the space out
        args = args.substr(0, args.length() - 1);
    }
    return args;
}

std::string get_color_key(std::vector<std::string> args)
{
    return args[0] + " " + args[1] + " " + args[2] + " " + args[3];
}

std::string generateRandomKey()
{
    std::string key;
    std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};

    // Seed the random number generator with the current time (once per program run)
    static bool seeded = false;
    if (!seeded)
    {
        std::srand((unsigned)std::time(nullptr));
        seeded = true;
    }

    for (int i = 0; i < 4; i++)
    {
        key += colors[(size_t)std::rand() % colors.size()];
    }
    validate_key(key);
    return key;
}

int get_playtime(std::string playtime)
{
    return std::stoi(playtime);
}

std::string currentDateTime()
{
    time_t fulltime;
    struct tm *current_time;
    char time_str[50];

    time(&fulltime);
    current_time = gmtime(&fulltime);
    sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", current_time->tm_year + 1900,
            current_time->tm_mon + 1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    return time_str;
}

std::string currentDateTimeFN()
{
    time_t fulltime;
    struct tm *current_time;
    char time_str[50];

    time(&fulltime);
    current_time = gmtime(&fulltime);
    sprintf(time_str, "%4d%02d%02d_%02d%02d%02d", current_time->tm_year + 1900,
            current_time->tm_mon + 1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    return time_str;
}

void setup_signal_handlers()
{
    // set SIGINT/SIGTERM handler to close server gracefully
    struct sigaction sa;

    // set the custom signal handler
    sa.sa_handler = terminate_signal_handler;

    // clear the signal set, so that no other signals are blocked
    sigemptyset(&sa.sa_mask);

    // no special flags
    sa.sa_flags = 0;

    // ctrl-c
    if (sigaction(SIGINT, &sa, NULL) == -1)
        throw UnrecoverableError("Setting SIGINT signal handler", errno);

    // signal to terminate program
    if (sigaction(SIGTERM, &sa, NULL) == -1)
        throw UnrecoverableError("Setting SIGTERM signal handler", errno);

    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

void terminate_signal_handler(int sig)
{
    // ignore the signal if the application is already shutting down
    (void)sig;
    if (is_exiting)
        exit(EXIT_SUCCESS);

    is_exiting = true;
}

std::string gameFileName(std::string PLID)
{
    return "GAME_" + PLID + ".txt";
}

int black(const std::string key, const std::string guess)
{
    int count = 0;
    for (std::string::size_type i = 0; i < key.size(); i++)
    {
        if (key[i] == guess[i])
        {
            count++;
        }
    }
    return count;
}

int white(const std::string key, const std::string guess)
{
    int whiteCount = 0;

    // store counts of colors that are unmatched in correct position
    std::unordered_map<char, int> secretFreq, guessFreq;

    // Loop through to first count the exact matches for black pieces
    for (std::string::size_type i = 0; i < key.size(); i++)
    {
        if (key[i] == guess[i])
        {
            // black piece
            continue;
        }
        else
        {
            // record the unmatched colors
            secretFreq[key[i]]++;
            guessFreq[guess[i]]++;
        }
    }

    // Calculate white pieces by comparing the frequencies
    for (const auto &entry : guessFreq)
    {
        char color = entry.first;
        int guessCount = entry.second;
        if (secretFreq.find(color) != secretFreq.end())
        {
            // The color exists in the secret, so add the minimum count of that color in guess and secret
            whiteCount += std::min(guessCount, secretFreq[color]);
        }
    }

    return whiteCount;
}

std::string playerDirectory(std::string PLID)
{
    return PLID;
}

int FindTopScores(SCORELIST *list)
{
    struct dirent **filelist;
    int nentries, ifile;
    char fname[300];
    FILE *fp;
    char mode[8];

    // Scan the directory and sort files alphabetically
    nentries = scandir(SCORES_DIR, &filelist, 0, alphasort);
    // If no entries are found, return 0
    if (nentries <= 0)
        return 0;

    else
    {
        ifile = 0;

        // Iterate through the entries from last to first
        while (nentries--)
        {
            // Check if the entry is not a hidden file and we haven't reached the top 10
            if (filelist[nentries]->d_name[0] != '.' && ifile < 10)
            {
                // Construct the file path
                sprintf(fname, "SCORES/%s", filelist[nentries]->d_name);

                // Open the file for reading
                fp = fopen(fname, "r");
                if (fp != NULL)
                {
                    // Read data from the file
                    if (!fscanf(fp, "%d %s %s %d %s",
                                &list->score[ifile],
                                list->PLID[ifile],
                                list->color_code[ifile],
                                &list->ntries[ifile],
                                mode))
                    {
                        fclose(fp); // error
                        continue;
                    }

                    // Parse the game mode
                    if (!strcmp(mode, "PLAY"))
                        list->mode[ifile] = MODEPLAY;
                    if (!strcmp(mode, "DEBUG"))
                        list->mode[ifile] = MODEDEBUG;

                    // Close the file
                    fclose(fp);
                    ++ifile;
                }
            }
            // Free the memory allocated for the entry
            free(filelist[nentries]);
        }
        // Free the file list
        free(filelist);
    }

    // Update the number of scores in the list
    return ifile;
}

int FindLastGame(int PLID, char *fname)
{
    struct dirent **filelist;
    int nentries, found;
    char dirname[20];

    sprintf(dirname, "GAMES/%d/", PLID);

    nentries = scandir(dirname, &filelist, 0, alphasort);
    found = 0;

    if (nentries <= 0)
    {
        return 0;
    }
    else
    {
        while (nentries--)
        {
            if (filelist[nentries]->d_name[0] != '.' && !found)
            {
                sprintf(fname, "GAMES/%d/%s", PLID, filelist[nentries]->d_name);
                found = 1;
            }
            free(filelist[nentries]);
        }
        free(filelist);
    }

    return found;
}
