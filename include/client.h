#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"

To_Player my_cards;

struct Table{
	char on_table[NUM_PLAYERS][MSG_SIZE];
	int currently;
}table;

Play recv_user ();
void* recv_func(void* arg);
void* send_func(void* arg);
void show_your_cards(const To_Player my_cards);
void show_table_cards();
void setup_client(const int port_number, int* socket_fd, struct sockaddr_in* server_addr, struct hostent* server);

#endif // !CLIENT_H
