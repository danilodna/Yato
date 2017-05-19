#ifndef SERVER_H
#define SERVER_H

#include "truco.h"



void init_deck();
void init_game (Game* game);
void init_round (Game* game);

int32_t RECV_VALID_CARD(Game game);
int32_t is_gameover(Game game);

void send_winner(const Game game, const int32_t winner);
void new_turn(Game* game);
void controller (Game* game, int32_t control);
void give_cards(Game* game);

void test_shuffle(Game* truco);
void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number);

#endif /* !SERVER_H */
