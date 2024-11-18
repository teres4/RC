#ifndef __UTILS_H__
#define __UTILS_H__


#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "constants.h"



int is_valid_port(char *);

int is_valid_plid(char *);

int setup_signal_handlers();
void signal_handler();


#endif