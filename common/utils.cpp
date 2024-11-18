#include <csignal>
#include "utils.hpp"

#include "protocols.hpp"

int is_shutting_down = 0;

void is_valid_port(std::string &port) {
    for (char c : port) {
        if (!std::isdigit(static_cast<unsigned char>(c))) 
            throw UnrecoverableError("Invalid port: not a number");
    }

    try {
        int32_t parsed_port = std::stoi(port);
        if (parsed_port < 1 || parsed_port > 65535) 
            throw std::runtime_error("");
    } catch (...) {
        throw UnrecoverableError(
            "Invalid port: it must be a number between 1 and 65535");
    }
}


int8_t validateUserID(std::string userID) {

  uint32_t id = (uint32_t)std::stoi(userID);
  if (id > USER_ID_MAX) {
    return INVALID;
  }

  return 0;
}


/**
 * Check if the plid is valid
*/
int is_valid_plid(std::string plid) {
    if (!is_digits(plid) || plid.length() != PLID_MAX_SIZE)
        return INVALID;

    uint32_t id = (uint32_t)std::stoi(plid);

    for (char c : plid) {
        if(!isdigit(c)) 
            throw UnrecoverableError("Invalid PLID: must be a number of 6 digits");
    }
}


uint32_t parse_packet_player_id(std::string &id_str) {
    if (id_str.length() != 6) 
        throw InvalidPacketException();
    
    for (char c : id_str) {
        if (!isdigit(c)) 
        throw InvalidPacketException();
    }

    try {
        int i = std::stoi(id_str);
        if (i < 0 || i > (int)PLAYER_ID_MAX) {
        throw InvalidPacketException();
        }
        return (uint32_t)i;
    } catch (...) {
        throw InvalidPacketException();
    }
}




void setup_signal_handlers() {
    // set SIGINT/SIGTERM handler to close server gracefully
    struct sigaction sa;

    // set the custom signal handler
    sa.sa_handler = signal_handler;

    // clear the signal set, so that no other signals are blocked
    sigemptyset(&sa.sa_mask);
    
    // no special flags
    sa.sa_flags = 0;

    // ctrl-c
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        throw UnrecoverableError("Setting SIGINT signal handler", errno);
    }
    // signal to terminate program
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        throw UnrecoverableError("Setting SIGTERM signal handler", errno);
    }

    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

void signal_handler(int sig) {
  (void)sig;
  if (is_shutting_down) {
    exit(EXIT_SUCCESS);
  }
  is_shutting_down = true;
}