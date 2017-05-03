#ifndef SERVER_H
#define SERVER_H

#include "truco.h"

Game truco;

int turn = 0;
int round_value = 2;
int score[NUM_PLAYERS];

Queue* queue;
void turn_alert(Play play);
void* recv_client(void* arg);
Play* parse_msg(const Play play);
int verify_turn(Play play);
Play parse_client(Play* play);
void control(Play* play);
void chat_msg(Play play);
int check_table(Game *game);
void new_turn(Play* play);
void manages_score(const int winner);
void init_game ();

void test_shuffle(Game* truco);
void init_deck();
void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number);

#endif /* !SERVER_H */
