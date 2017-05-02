#include "../include/client.h"

int main(int argc, char** argv) {
	verify_args_client(argc, argv);

	// THREAD VARIABLES =============
	pthread_t t_recv, t_send;
	// ==============================

	// CLIENT VARIABLES =============
	int socket_fd;	// Socket File Descriptors to be used in the process conection
	struct sockaddr_in server_addr;
	struct hostent* server = gethostbyname(argv[1]);
	const int port_number = atoi(argv[2]); // Stores the port number on which the server accepts connections.
	// ==============================
	if (server == NULL)
  	error("ERROR, no such host\n");

	setup_client(port_number, &socket_fd, &server_addr, server);

	// Estabelishing connection with server
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  	error("ERROR connecting\n");

	// Creating thread to receive data from server.
	if(pthread_create(&t_recv, NULL, recv_func, &(socket_fd)) != 0){
		printf("Failure creating thread to receive data. Excecution terminated.");
		exit(EXIT_FAILURE);
	}

	// Creating thread to data data to the server.
	if(pthread_create(&t_send, NULL, send_func, &(socket_fd)) != 0){
		printf("Failure creating thread to send data. Excecution terminated.");
		exit(EXIT_FAILURE);
	}

	pthread_join(t_recv, NULL);
	pthread_join(t_send, NULL);

	// Extremely important to close the stream
	close(socket_fd);
	return EXIT_SUCCESS;
}

// ==================================================================

void* recv_func(void* arg){
	int desc = *((int *) arg);

	my_cards = recv_cards(desc);
	printf("Olá, jogador %d:\n", my_cards.id + 1);
	show_table_cards();
	show_your_cards(my_cards);

	/*
	memset((char *) buffer, 0, sizeof(buffer));
	n = read(desc, buffer, sizeof(buffer));
	check_socket_io(n);

	int p_turn = strtoimax(buffer, NULL, 10);


	if(is_my_turn(p_turn)){
		printf("É o seu turno!\n");
	} else {
		printf("É o turno do jogador %d.\n", p_turn + 1);
	}
	*/
	Play play;
	while(1){
		play = recv_server(desc);

		// PRINT SAFADO
		printf("Jogador %d disse: %s", play.player + 1, play.msg);
	}

	return NULL;
}

int is_my_turn(const int player_turn){
	return player_turn == my_cards.id;
}

void* send_func (void* arg) {
	int desc = *((int *) arg);
	int n;

	while(1) {
		Play play = recv_user();

		// ID of the player
		n = write(desc, &play.player, sizeof(play.player));
		check_socket_io(n);

		// Card IDs
		n = write(desc, &play.card_id, sizeof(play.card_id));
		check_socket_io(n);

		// Card names
		n = write(desc, &play.msg, sizeof(play.msg));
		check_socket_io(n);
	}

	return NULL;
}

Play recv_user() {
	int i, j;
	char buffer[MSG_SIZE], tmp[MSG_SIZE], tmp2[MSG_SIZE];
	Play play;
	play.player = my_cards.id;

	//	Get the input of the player
	fgets(buffer, MSG_SIZE, stdin);
	strcpy(tmp, buffer);

	// Transforms it all to lower case
	for(i = 0; buffer[i]; ++i){
  	tmp[i] = toupper(buffer[i]);
	}

	// Make comparisons with the cards on hand
	// If it matches with the cards in his hand, is his play.
	// If matches that will be his play
	// Otherwise it can be a 'TRUCO' call or a chat message
	for (i = 0; i < HAND; ++i){
		for (j = 0; my_cards.card_name[i][j]; ++j) {
			tmp2[j] = toupper(my_cards.card_name[i][j]);
		}

		if(strcmp(tmp, tmp2) == 0) {
			play.card_id = my_cards.card_id[i];
			strcpy(play.msg, my_cards.card_name[i]);
			return play;
		}
	}

	//	Case player asks for 'Truco'
	if (strcmp(tmp, "TRUCO") == 0){
		strcpy(play.msg, tmp);
		return play;
	}

	//	Otherwise will be a chat message, thus we concatenate ':'
	//	that is a chat message at the begining of the string to signal.
	strcpy(play.msg, ":");
	strcat(play.msg, buffer);

	return play;
}

Play recv_server (const int sock_fd) {
	int n;
	Play data;
	n = read(sock_fd, &data.player, sizeof(data.player));
	check_socket_io(n);

	n = read(sock_fd, &data.card_id, sizeof(data.card_id));
	check_socket_io(n);

	n = read(sock_fd, &data.msg, sizeof(data.msg));
	check_socket_io(n);

	return data;
}

To_Player recv_cards (const int sock_fd) {
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

void show_table_cards(){
	int i;
	printf("Cartas na mesa:\n");
	for(i = 0; i < table.currently; ++i){
		printf("%s\n", table.on_table[i]);
	}
	printf("\n");
}

void show_your_cards(const To_Player my_cards){
	int i;
	printf("Suas cartas:\n");
	for (i = 0; i < HAND; i++) {
		if(my_cards.card_id[i] != -1)
			printf("%s\n", my_cards.card_name[i]);
	}
}

// ===========================================================================================================
// ===========================================================================================================

void setup_client(const int port_number, int* socket_fd, struct sockaddr_in* server_addr, struct hostent* server){
	// AF => Adress Family; INET => InterNET
	// SOCK_STREAM => TCP or SOCK_DGRAM => UDP
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socket_fd < 0)
		error("ERROR opening socket\n");

	memset((void *) server_addr, 0, sizeof(*server_addr));
	server_addr->sin_family = AF_INET;
	memmove((void *) &server_addr->sin_addr.s_addr, (const void *) server->h_addr, server->h_length);
	server_addr->sin_port = htons(port_number);

	/*
	struct hostent {
  	char    *h_name;									Official name of host
  	char    **h_aliases;							Alias list
  	int     h_addrtype;								Host address type
  	int     h_length;									Length of address
  	char    **h_addr_list;						List of addresses from name server
  	#define h_addr  h_addr_list[0]		Address, for backward compatiblity

	};
		h_name       Official name of the host.
		h_aliases    A zero  terminated  array  of  alternate names for the host.
		h_addrtype   The  type  of  address  being  returned; currently always AF_INET.
		h_length     The length, in bytes, of the address.
		h_addr_list  A pointer to a list of network addresses for the named host. Host addresses are returned in network byte order.
	*/
}
