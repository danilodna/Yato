#ifndef SERVER_H
#define SERVER_H

#include "truco.h"

#define QUEUE_SIZE 1000

Queue* queue;

void test_shuffle();
void* chatty(void* arg);
void* polling(void* arg);
void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number);

#endif /* !SERVER_H */
