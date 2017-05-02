#ifndef SERVER_H
#define SERVER_H

#include "truco.h"

Game truco;

Queue* queue;
int turn = 0;

void send_to_client(const int sock_fd, const Play play);
void parse_msg(const Play play);
//void new_turn(const Player* const p);
void send_cards(int sock_fd, const To_Player to_player);
void init_deck();
void* game_rolling(void* arg);
void test_shuffle(Game* truco);
void* recv_client(void* arg);
//void* polling(void* arg);
void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number);

#endif /* !SERVER_H */
