#include "utils.hpp"
#include "protocols.hpp"

#include <iostream>

int is_exiting = 0;

void validate_port(std::string &port)
{
    for (char c : port)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw UnrecoverableError("Invalid port: not a number");
        }
    }

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

int validate_ip(std::string ip)
{
    // check if the ip is valid
    std::cout << ip;
    return VALID;
}

/**
 * Check if the plid is valid
 */
int validate_plid(std::string plid)
{
    if (plid.length() != PLID_MAX_SIZE)
        return INVALID;

    for (char c : plid)
    {
        if (!std::isdigit(c))
            return INVALID;
    }

    int parsed = std::stoi(plid);
    if (parsed < 0)
        return INVALID;

    return parsed;
}

int validatePlayTime(std::string playtime)
{
    if (playtime.length() > MAX_PLAYTIME)
        return INVALID;

    for (char c : playtime)
    {
        if (!std::isdigit(c))
            return INVALID;
    }
    int i = std::stoi(playtime);

    if (i < 0)
        return INVALID;

    return VALID;
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
