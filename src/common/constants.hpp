#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT "58013" // 5800 + GROUP NUMBER

#define MAX_TRIALS 8
#define PLID_MAX_SIZE 6
#define MAX_PLAYTIME 600
#define MAX_PLAYTIME_DIGITS 3

#define MAX_FNAME 24
#define MAX_FSIZE 1024
#define MAX_TRANSMISSION 4

#define BUFFER_SIZE 128

#define PROTOCOL_ERROR "ERR"

#define SOCKETS_UDP_TIMEOUT 5
#define RESEND_TRIES 5
#define TCP_READ_TIMEOUT 30
#define TCP_WRITE_TIMEOUT 300

#define GAME_FILES_DIR "./src/game_files/"
#define FILES_DIR "./src/gamedata/"
#define GAMES_DIR "./src/gamedata/GAMES/"
#define SCORES_DIR "./src/gamedata/SCORES/"

#define WIN_CODE "W"
#define FAIL_CODE "F"
#define QUIT_CODE "Q"
#define TIMEOUT_CODE "T"

#define MODEPLAY 0
#define MODEDEBUG 1

#endif