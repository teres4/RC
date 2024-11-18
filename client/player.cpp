#include "player.hpp"
#include "common/utils.hpp"


extern bool is_exiting; // flag to indicate whether the application is exiting



int main(int argc, char *argv[])
{
    setup_signal_handlers();  // change the signal handlers to our own
    ClientConfig config(argc, argv); // parse command-line arguments


    return 0;
}