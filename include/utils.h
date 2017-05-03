#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include "safe.h"

#define QUEUE_SIZE 1000
#define MSG_SIZE 256
#define HAND 3
#define NUM_PLAYERS 2
#define NUM_CARDS 40
#define INVALID -1



typedef struct {
	int player;
	int card_id;
	char msg[MSG_SIZE];
	char flag;
}Play;

typedef struct {
	int id;
	int card_id[HAND];
	char card_name[HAND][MSG_SIZE];
}To_Player;

typedef struct Queue {
	Play* content;
  int begin;
  int end;
	int size;
	int max;
} Queue;

Queue* create_queue(int size);
void destroy_queue(Queue* queue);
int is_queue_empty(Queue* queue);
int is_queue_full(Queue* queue);
void add_queue(Queue* queue, Play play);
Play remove_queue(Queue* queue);
void print_first(Queue* queue);

void serialize_play (const int sock_fd, const Play play);
Play deserialize_play (const int sock_fd);

void serialize_cards(const int sock_fd, const To_Player to_player);
To_Player deserialize_cards (const int sock_fd);

void error(const char *msg);
void check_socket_io(const int n);
void verify_args_client(int argc, char* argv[]);
void verify_args_server(int argc, char* argv[]);

#endif // !UTILS_H
