#include "../include/server.h"


int main(int argc, char** argv) {
	verify_args_server(argc, argv);

	int i;
	// GAME VARIABLES ===============
	Game truco;
	queue = create_queue(QUEUE_SIZE);
	// ==============================

	// SERVER VARIABLES =============
	int socket_fd; // Socket File Descriptors to be used in the process conection
	const int port_number = atoi(argv[1]); // Port number
	// int n;  return value for the read() and write() calls; i.e. it contains the number of characters read or written.
	socklen_t client_length; // Stores the size of the address of the client. This is needed for the accept system call.
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	// ==============================

	// THREAD VARIABLES =============
	pthread_t THREAD[NUM_PLAYERS], dist_data;
	if(pthread_mutex_init(&mutex, NULL) != 0) {
		error("Mutex initiation failed, execution aborted.");
	}
	// ==============================

	setup_server(&server_addr, &socket_fd, port_number);
	printf("Server setup complete.\n");

	// List with connections made to the socket_fd.
	// Max size is 5
	listen(socket_fd,5);

	printf("Wating for players to connect...\n");
	// In the routine bellow the program halts until a connections with the client is estabelished
	client_length = sizeof(client_addr);
	for(i = 0; i < NUM_PLAYERS; ++i) {

		// Accepting incoming clients connections
		truco.player[i].sock_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
		if (truco.player[i].sock_fd < 0)
	  	error("ERROR on accept");

		// Creating thread to deal with the clients incoming clients connections
		if(pthread_create(&THREAD[i], NULL, chatty, &(truco.player[i])) != 0) {
			printf("Failure creating thread number %d, excecution terminated.", i);
			exit(EXIT_FAILURE);
		}

		printf("Jogador %d conectou-se...\n", i + 1);
	}

	printf("Todos os jogadores conectados...\n");

	for(i = 0; i < NUM_PLAYERS; ++i)
		printf("%d\n", truco.player[i].sock_fd);


	if(pthread_create(&dist_data, NULL, polling, &truco) != 0) {
		printf("Failure creating thread number %d, excecution terminated.", i);
		exit(EXIT_FAILURE);
	}

	// Execution of main on the server wait here for all threads to finish their tasks.
	for(i = 0; i < NUM_PLAYERS; ++i) {
		pthread_join(THREAD[i], NULL);
	}
	pthread_join(dist_data, NULL);

	// Closing sockets
	for(i = 0; i < NUM_PLAYERS; i++)
		close(truco.player[i].sock_fd);
	close(socket_fd);
	return 0;
}

// ==================================================================

void test_shuffle() {
	srand(time(NULL));   // should only be called once

	Game truco;

	shuffling_deck(&truco);

	int i, j;
	for(i = 0; i < NUM_PLAYERS; ++i){
		for(j = 0; j < HAND; ++j){
			printf("Player %d - Card %d == %d\n", i, j, truco.player[i].card_id[j]);
		}
		printf("\n");
	}
}

void* chatty(void* arg) {
	Player* player = (Player*) arg;
	int n;
	char buffer[MSG_SIZE]; // The server reads characters from the socket connection into this buffer.

	while(1){
		memset((char *) buffer, 0, sizeof(buffer));
		n = read(player->sock_fd, buffer, sizeof(buffer));
		check_socket_io(n);

		pthread_mutex_lock(&mutex);
		add_queue(queue, buffer);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

void* polling(void* arg) {
	Game* truco = (Game*) arg;
	int i, n;
	char buffer[MSG_SIZE];

	for(;;){
		if(!is_queue_empty(queue)){
			memset((char *) buffer, 0, sizeof(buffer));
			pthread_mutex_lock(&mutex);
			strcpy(buffer, remove_queue(queue));
			pthread_mutex_unlock(&mutex);

			for(i = 0; i < NUM_PLAYERS; ++i){
				n = write(truco->player[i].sock_fd, &buffer, sizeof(buffer));
				check_socket_io(n);
			}
		}
	}

	return NULL;
}


void setup_server(struct sockaddr_in* server_addr, int* socket_fd, const int port_number) {

	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
  	error("ERROR opening socket");

	memset((char *) server_addr, 0, sizeof(*server_addr)); // Sets all values in a buffer to zero.

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(port_number);
	server_addr->sin_addr.s_addr = INADDR_ANY;

	if (bind(*socket_fd, (struct sockaddr *) server_addr, sizeof(*server_addr)) < 0)
  	error("ERROR on binding");


	//	A sockaddr_in is a structure containing an internet address. This structure is defined in netinet/in.h
	//
	//	struct sockaddr_in {
  //		short   sin_family; must be AF_INET
  //		u_short sin_port;
  //		struct  in_addr sin_addr;
  //		char    sin_zero[8]; Not used, must be zero
	//	};
	//
	// AF => Adress Family; INET => InterNET
	// SOCK_STREAM => TCP or SOCK_DGRAM => UDP
}
