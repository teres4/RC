#include "utils.hpp"
#include "protocol.hpp"

#include <iostream>

int is_exiting = 0;

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
    if (plid.length() != PLID_MAX_SIZE || is_numeric(plid) == false){
        throw ProtocolMessageErrorException();
    }
        
    int parsed = std::stoi(plid);
    if (parsed < 0){
        throw ProtocolMessageErrorException();
    }
}

int get_plid(std::string plid)
{
    return std::stoi(plid);
}

void validate_playTime(std::string playtime)
{
    if (is_numeric(playtime) == false){
        throw ProtocolMessageErrorException();
    }

    int parsed = std::stoi(playtime);

    if (parsed < 0 || parsed > MAX_PLAYTIME){
        throw ProtocolMessageErrorException();
    }
}

void validate_playTime(int playtime)
{
    if (playtime < 0 || playtime > MAX_PLAYTIME){
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
    return args;
}

std::string get_color_key(std::vector<std::string> args)
{
    return args[0] + " " + args[1] + " " + args[2] + " " + args[3];
}

void generateRandomKey(std::string key)
{
    std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};
    for (int i = 0; i < 4; i++)
    {
        key += colors[(size_t)(rand() % 6)];
    }
    
    validate_key(key);

}

int get_playtime(std::string playtime)
{
    return std::stoi(playtime);
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