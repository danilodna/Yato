#ifndef UTILS_H
#define UTILS_H

#include "safe.h"
#define MSG_SIZE 256

typedef struct Queue {
	char** content;
  int begin;
  int end;
	int size;
	int max;
} Queue;

Queue* create_queue(int size);
void destroy_queue(Queue* queue);
int is_queue_empty(Queue* queue);
int is_queue_full(Queue* queue);
void add_queue(Queue* queue, char *str);
char* remove_queue(Queue* queue);
void print_first(Queue* queue);

void error(const char *msg);
void check_socket_io(const int n);
void verify_args_client(int argc, char* argv[]);
void verify_args_server(int argc, char* argv[]);



#endif // !UTILS_H
