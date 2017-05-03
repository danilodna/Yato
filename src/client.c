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

void* send_func (void* arg) {
	int desc = *((int *) arg);

	while(1) {
		Play play = recv_user();
		play.player = my_cards.id;
		printf("MANDEI MENSAGEM PRO SERVIDOR!\n");
		serialize_play(desc, play);
	}

	return NULL;
}

void* recv_func(void* arg) {
	int desc = *((int *) arg);

	my_cards = deserialize_cards(desc);

	printf("Olá, jogador %d:\n", my_cards.id + 1);
	new_turn();
	int aux = current_turn;

	while(1){
		if(aux != current_turn){
			aux = current_turn;
			new_turn();
		}
		Play play = deserialize_play(desc);
		control(&play);
	}

	return NULL;
}



Play recv_user() {
	Play play;
	fgets(play.msg, MSG_SIZE, stdin);
	int len = strlen(play.msg);
	play.msg[--len] = '\0';
	return play;
}

void control(Play *play) {
	int protocol;

	if(play->flag >= 0 && play->flag < NUM_PLAYERS) {
		update_turn(play->flag);
		protocol = 0;
	} else {
		protocol = play->flag;
	}

	switch(protocol) {
		case 0: {
			update_status(play);
		}
		break;

		case 1:
			printf("CASE 2 CLIENT PROTOCOL.\n");
		break;

		case 2:
			printf("CASE 2 CLIENT PROTOCOL.\n");
		break;

		case 3:
			printf("CASE 3 CLIENT PROTOCOL.\n");
		break;

		case 'a':
			printf("Jogador %d disse: %s\n", play->player + 1, play->msg);
		break;

		default:

		break;
	}
}

void new_turn(){
	printf("\n======= TURNO ATUAL: %d =======\n\n", current_turn);
	show_table_cards();
	show_your_cards(my_cards);
	printf("\n===============================\n\n");
}

void update_turn(int new_turn){
	current_turn = new_turn;
}

void update_status(Play* play){
	strcpy(table.on_table[table.currently], play->msg);
	table.currently++;

	if(my_cards.id == play->player) {
		int i;
		for(i = 0; i < HAND; ++i){
			if(play->card_id == my_cards.card_id[i]) {
				my_cards.card_id[i] = -1;
				strcpy(my_cards.card_name[i], "-----");
			}
		}
	}
}

void show_table_cards(){
	int i;
	printf("=== Cartas na mesa ===\n");
	for(i = 0; i < table.currently; ++i){
		printf("%s\n", table.on_table[i]);
	}
	printf("\n");
}

void show_your_cards (const To_Player my_cards) {
	int i;
	printf("=== Suas cartas ===\n");
	for (i = 0; i < HAND; i++) {
		if(my_cards.card_id[i] != -1)
			printf("%s - id: %d\n", my_cards.card_name[i], my_cards.card_id[i]);
	}
}

void show_score(const To_Player my_cards) {
	int i;
	for(i = 0; i < NUM_PLAYERS; ++i){

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

/*
Play recv_user() {
	int i, j;
	char buffer[MSG_SIZE], tmp[MSG_SIZE], tmp2[MSG_SIZE];
	Play play;

	//	Get the input of the player without the newline in the end
	fgets(buffer, MSG_SIZE, stdin);
	strncpy(tmp, buffer, strlen(buffer) - 1);

	// Transforms it all to upper case
	for(i = 0; tmp[i]; ++i){
  	tmp[i] = toupper(tmp[i]);
	}

	// Make comparisons with the cards on hand
	// If it matches with the cards in his hand, is his play.
	// If matches that will be his play
	// Otherwise it can be a 'TRUCO' call or a chat message
	for (i = 0; i < HAND; ++i){
		strcpy(tmp2, my_cards.card_name[i]);
		for (j = 0; j < (int) strlen(tmp2); ++j) {
			tmp2[j] = toupper(tmp2[j]);
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
	strcpy(play.msg, ": ");
	strcat(play.msg, buffer);

	return play;
}
*/
