#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <stdexcept>
#include <csignal>

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
bool validate_plid(std::string plid);
bool validatePlayTime(std::string playtime);
bool validate_color(std::string color);


int get_plid(std::string plid);
int get_playtime(std::string playtime);
std::string get_color_key(std::string args);



void setup_signal_handlers();
void terminate_signal_handler(int sig);

#endif