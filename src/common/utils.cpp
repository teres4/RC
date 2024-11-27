#include "utils.hpp"
#include "protocol.hpp"

#include <iostream>

int is_exiting = 0;

int is_numeric(std::string &str){
    for (char c : str)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return INVALID;
        }
    }
    return VALID;
}


int is_not_numeric(std::string &str){
    if (is_numeric(str) == VALID)
        return INVALID;

    return VALID;
}


void validate_port(std::string &port)
{
    if (is_numeric(port) == INVALID)
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
int validate_plid(std::string plid)
{
    if (plid.length() != PLID_MAX_SIZE || is_numeric(plid) == INVALID)
        return INVALID;

    int parsed = std::stoi(plid);
    if (parsed < 0)
        return INVALID;

    return VALID;
}

int get_plid(std::string plid){
    return std::stoi(plid);
}

int validatePlayTime(std::string playtime)
{
    if (is_numeric(playtime) == INVALID)
        return INVALID;

    int parsed = std::stoi(playtime);

    if (parsed < 0 || parsed > MAX_PLAYTIME)
        return INVALID;

    return VALID;
}


int get_playtime(std::string playtime){
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
