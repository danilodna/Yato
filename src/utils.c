#include "../include/utils.h"

// Global queue functions ================================

Queue* create_queue(int size) {
	int i;
	Queue* queue;
	queue = (Queue*) safe_malloc(sizeof(Queue));
	queue->content = (char **) safe_calloc(size, sizeof(char *));
	for(i = 0; i < size; ++i){
		queue->content[i] = (char *) safe_calloc(MSG_SIZE, sizeof(char));
	}
	if(queue != NULL || queue->content != NULL) {
		queue->begin = -1;
		queue->end = -1;
		queue->size = 0;
		queue->max = size;
	} else
		error("Não foi possível criar a fila.");

	return queue;
}

void destroy_queue(Queue* queue) {
	int i;

	for(i = 0; i < queue->size; ++i){
		free(queue->content[i]);
	}

	free(queue);
}

int is_queue_empty(Queue* queue) {
	return queue->size <= 0;
}

int is_queue_full(Queue* queue) {
	return queue->size >= queue->max;
}

void add_queue(Queue* queue, char *str) {
	if (is_queue_empty(queue)){
		queue->begin = -1;
		queue->end = -1;
	}
	if (is_queue_full(queue))
    error("Queue Overflow.\n");
	else{
		if(queue->begin == -1)
			queue->begin = 0;

		if(queue->end >= (queue->max) - 1)
			queue->end = 0;
		else
			++(queue->end);

		++(queue->size);
		strcpy(queue->content[queue->end], str);
	}
}

char* remove_queue(Queue* queue) {
	char* temp;

	if (is_queue_empty(queue))
    error("Queue Underflow.\n");
	else {
		temp = queue->content[queue->end];

		if(queue->begin >= (queue->max) - 1)
			queue->begin = 0;
		else
			++(queue->begin);
	}
	--(queue->size);
	return temp;
}

void print_first(Queue* queue){
	fputs(queue->content[queue->begin], stdout);
}

// Verify args function ========================================

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_socket_io(const int n) {
	if(n < 0)
		error("ERROR reading or writing from socket");
}

void verify_args_client(int argc, char* argv[]) {
	if(argc != 3){
		printf("Wrong arguments, the correct form is:\n");
		printf("./server localhost port_number\n");
		exit(EXIT_FAILURE);
	}
}

void verify_args_server(int argc, char* argv[]) {
	if(argc != 2){
		printf("Wrong arguments, the correct form is:\n");
		printf("./server port_number\n");
		exit(EXIT_FAILURE);
	}
}
