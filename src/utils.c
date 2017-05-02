#include "../include/utils.h"

// Global queue functions ================================

Queue* create_queue(int size) {
	Queue* queue;
	queue = (Queue*) safe_malloc(sizeof(Queue));
	queue->content = (Play *) safe_calloc(size, sizeof(Play));
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
		free(queue->content->msg);
	}
	free(queue->content);
	free(queue);
}

int is_queue_empty(Queue* queue) {
	return queue->size <= 0;
}

int is_queue_full(Queue* queue) {
	return queue->size >= queue->max;
}

void add_queue(Queue* queue, Play play) {
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
		queue->content[queue->end].player = play.player;
		queue->content[queue->end].card_id = play.card_id;
		strcpy(queue->content[queue->end].msg, play.msg);
	}
}

Play remove_queue(Queue* queue) {
	Play temp;

	if (is_queue_empty(queue))
    error("Queue Underflow.\n");
	else {
		temp.player = queue->content[queue->end].player;
		temp.card_id = queue->content[queue->end].card_id;
		strcpy(temp.msg, queue->content[queue->end].msg);

		if(queue->begin >= (queue->max) - 1)
			queue->begin = 0;
		else
			++(queue->begin);
	}
	--(queue->size);
	return temp;
}

void print_first(Queue* queue){
	fputs(queue->content[queue->begin].msg, stdout);
}

// Serialize and Deserialize struct ========================================

void serialize_play (const int sock_fd, const Play play) {
	int n;

	// Sending the data about the play that the player made

	// ID of the player
	n = write(sock_fd, &play.player, sizeof(play.player));
	check_socket_io(n);

	// Card ID
	n = write(sock_fd, &play.card_id, sizeof(play.card_id));
	check_socket_io(n);

	// Card name
	n = write(sock_fd, &play.msg, sizeof(play.msg));
	check_socket_io(n);

	// Flag for the current player turn
	n = write(sock_fd, &play.its_your_turn, sizeof(play.its_your_turn));
	check_socket_io(n);

	// Flag for to warn the player that the play is valid
	n = write(sock_fd, &play.valid_play, sizeof(play.valid_play));
	check_socket_io(n);

}

Play deserialize_play (const int sock_fd) {
	int n;
	Play play;

	// Receiving play

	// ID of the player
	n = read(sock_fd, &play.player, sizeof(play.player));
	check_socket_io(n);

	// Card ID
	n = read(sock_fd, &play.card_id, sizeof(play.card_id));
	check_socket_io(n);

	// Card name
	n = read(sock_fd, &play.msg, sizeof(play.msg));
	check_socket_io(n);

	// Flag for the current player turn
	n = read(sock_fd, &play.its_your_turn, sizeof(play.its_your_turn));
	check_socket_io(n);

	// Flag for to warn the player that the play is valid
	n = read(sock_fd, &play.valid_play, sizeof(play.valid_play));
	check_socket_io(n);

	return play;
}

void serialize_cards(const int sock_fd, const To_Player to_player){
	int i, n;

	// Sending the initial and essential data to players

	// ID of the player
	n = write(sock_fd, &to_player.id, sizeof(to_player.id));
	check_socket_io(n);

	// Card IDs
	for(i = 0; i < HAND; ++i){
		n = write(sock_fd, &to_player.card_id[i], sizeof(to_player.card_id[i]));
		check_socket_io(n);
	}

	// Card names
	for(i = 0; i < HAND; ++i){
		n = write(sock_fd, &to_player.card_name[i], sizeof(to_player.card_name[i]));
		check_socket_io(n);
	}
}

To_Player deserialize_cards (const int sock_fd) {
	int i, n;
	To_Player data;
	n = read(sock_fd, &data.id, sizeof(data.id));
	check_socket_io(n);

	for(i = 0; i < HAND; ++i){
		n = read(sock_fd, &data.card_id[i], sizeof(data.card_id[i]));
		check_socket_io(n);
	}

	for(i = 0; i < HAND; ++i){
		n = read(sock_fd, &data.card_name[i], sizeof(data.card_name[i]));
		check_socket_io(n);
	}

	return data;
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
