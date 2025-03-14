#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <stdexcept>
#include <csignal>
#include <vector>

#include "constants.hpp"

class UnrecoverableError : public std::runtime_error
{
public:
    UnrecoverableError(const std::string &__what) : std::runtime_error(__what) {}
    UnrecoverableError(const std::string &__what, const int __errno)
        : std::runtime_error(__what + ": " + strerror(__errno)) {}
};

bool is_numeric(std::string &str);
bool is_not_numeric(std::string &str);

void validate_port(std::string &port);
void validate_plid(std::string plid);

void validate_playTime(std::string playtime);
void validate_playTime(int playtime);

bool validate_color(std::string color);
void validate_key(std::vector<std::string> colors);
void validate_key(std::string key);

int get_plid(std::string plid);
int get_playtime(std::string playtime);
std::string get_color_key(std::string args);
std::string get_color_key(std::vector<std::string> args);

std::string currentDateTime();
std::string currentDateTimeFN();
std::string truncateDate(std::string datetime);

void setup_signal_handlers();
void terminate_signal_handler(int sig);

std::string generateRandomKey();
std::string gameFileName(std::string PLID);
std::string playerDirectory(std::string PLID);

int black(const std::string key, const std::string secretKey);
int white(const std::string key, const std::string secretKey);

typedef struct
{
    int score[10];
    char PLID[10][7];
    char color_code[10][5];
    int ntries[10];
    int mode[10];
} SCORELIST;

int findTopScores(SCORELIST *list);

void findLastGame(std::string plid, std::string &path);

#endif