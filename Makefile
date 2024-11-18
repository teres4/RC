# Makefile

CC = gcc

CFLAGS_SERVER = -Wall -Wextra -Werror -Wunused -pedantic -pthread
CFLAGS_CLIENT = -Wall -Wextra -Werror -Wunused -pedantic
# CFLAGS_COMMON = -Wall -Wextra -Werror -Wunused -pedantic

SRC_CLIENT = client/*.c
SRC_SERVER = server/*.c
# SRC_COMMON = common/*.c

TARGET_CLIENT = player
TARGET_SERVER = GS
# TARGET_COMMON = util

ALL: $(SRC) $(SRC_SERVER)
# 	$(CC) $(CFLAGS_COMMON) $(SRC_COMMON) -o $(TARGET_COMMON)
	$(CC) $(CFLAGS_CLIENT) $(SRC_CLIENT) -o $(TARGET_CLIENT)
	$(CC) $(CFLAGS_SERVER) $(SRC_SERVER) -o $(TARGET_SERVER)

clean:
	rm -f $(TARGET_CLIENT) $(TARGET_SERVER) 



