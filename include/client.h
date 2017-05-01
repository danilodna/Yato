#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"

void* recv_func(void* arg);
void* send_func(void* arg);
void setup_client(const int port_number, int* socket_fd, struct sockaddr_in* server_addr, struct hostent* server);

#endif // !CLIENT_H
