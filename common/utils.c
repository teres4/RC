#include "utils.h"

int is_shutting_down = 0;

/**
Check if port string is a valid port number 
*/
int is_valid_port(char *port) {
    int port_int = atoi(port);  // atoi returns 0 on error, 
    
    // check lower and upper limits of port numbers ]0, 65536[
    // [1, 65535]
    if (port_int < 1 || port_int > 65535)
        return 0;

    return 1;
}


/**
 * Check if the plid is valid
*/
int is_valid_plid(char *plid){
    if (strlen(plid) != PLID_SIZE)
        return 0;

    for (int i = 0; i < PLID_SIZE; ++i) {
        if(!isdigit(plid[i])) return 0;
    }

    return 1;
}


int setup_signal_handlers() {
    // set SIGINT/SIGTERM handler to close server gracefully
    struct sigaction sa;

    // set the custom signal handler
    sa.sa_handler = &signal_handler;
    // clear the signal set, so that no other signals are blocked
    sigemptyset(&sa.sa_mask);
    // no special flags
    sa.sa_flags = 0;

    // ctrl-c
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        return 1;
    }
    // signal to terminate program
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        return 1;
    }

    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    return 0;
}

void signal_handler() {
    if (is_shutting_down) {
        exit(EXIT_SUCCESS);
    }
    is_shutting_down = 1;
}




