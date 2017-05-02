#ifndef SERVER_H
#define SERVER_H

#include "truco.h"

Game truco;

Queue* queue;
int turn = 0;
int round_value = 2;
int score[NUM_PLAYERS];

void* game_rolling(void* arg);
void* recv_client(void* arg);
void parse_msg(const Play play);

void test_shuffle(Game* truco);
void init_deck();
void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number);

#endif /* !SERVER_H */
