#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include "safe.h"

#define CARD_NAME_SIZE 32
#define HAND 3
#define NUM_PLAYERS 2
#define NUM_CARDS 40
#define INVALID -1
#define MAX_POINTS 12
#define BUFFER_SIZE 1024

enum {
	FALSE = 0,
	TRUE = 1,
	DRAW = -1,
	YOUR_TURN = 40,
	ROUND_WINNER = 50,
	HAND_WINNER = 60
};

void send_card(const int32_t sock_fd, int32_t value);
int32_t recv_card(const int32_t sock_fd);

void error(const char *msg);
void check_socket_io(const int n);
void verify_args_client(int argc, char* argv[]);
void verify_args_server(int argc, char* argv[]);

#endif // !UTILS_H
