#include "../include/server.h"

int main(int argc, char** argv) {
	verify_args_server(argc, argv);
	// GAME VARIABLES ===============
	queue = create_queue(QUEUE_SIZE);
	int i;
	srand(time(NULL));   // should only be called once
	// ==============================

	init_deck();
	shuffling_deck(&truco);

	// SERVER VARIABLES =============
	int socket_fd; // Socket File Descriptors to be used in the process conection
	const int port_number = atoi(argv[1]); // Port number
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

	// List with connections made to the socket_fd. Max size is 5
	listen(socket_fd, 5);
	printf("Wating for players to connect...\n");

	//	In the routine bellow the program halts until a connections with the client is estabelished
	client_length = sizeof(client_addr);
	for(i = 0; i < NUM_PLAYERS; ++i) {

		//	Accepting incoming clients connections
		truco.player[i].sock_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
		if (truco.player[i].sock_fd < 0)
	  	error("ERROR on accept");

		//	Give the player an ID
		truco.player[i].id = i;

		//	For each client, we will create a thread to deal with incoming messages
		if(pthread_create(&THREAD[i], NULL, recv_client, &(truco.player[i])) != 0) {
			printf("Failure creating the thread number %d, that handle client messages.\nExcecution terminated.\n", i);
			exit(EXIT_FAILURE);
		}

		printf("Jogador %d conectou-se...\n", i + 1);
	}

	printf("Todos os jogadores conectados!\n\n");
	printf("=====================================================\n\n");

	init_game ();

	//	GAME LOOP
	for(;;){
		if(!is_queue_empty(queue)) {
			pthread_mutex_lock(&mutex);
			Play play = remove_queue(queue);
			pthread_mutex_unlock(&mutex);
			parse_client(&play);
			control(&play);
		}
	}

	// Execution of main on the server wait here for all threads to finish their tasks.
	for(i = 0; i < NUM_PLAYERS; ++i) {
		pthread_join(THREAD[i], NULL);
	}
	pthread_join(dist_data, NULL);

	printf("=====================================================\n");
	printf("\t\t\t\tJogo terminado, obrigado por jogar!\n");
	printf("=====================================================\n\n");

	//	Closing sockets
	for(i = 0; i < NUM_PLAYERS; i++)
		close(truco.player[i].sock_fd);
	close(socket_fd);
	return EXIT_SUCCESS;
}

// ==============================================================================================
// ==============================================================================================

void init_game () {
	int i, j, aux;
	To_Player to_player[NUM_PLAYERS];
	shuffling_deck(&truco);
	for(i = 0; i < NUM_PLAYERS; ++i){
		to_player[i].id = truco.player[i].id;
		// This loops give to eache player his cards for this round
		for(j = 0; j < HAND; ++j) {
			// Set 'aux' value to be the card ID that was assign to each player
			aux = truco.player[i].card_id[j];

			to_player[i].card_id[j] = aux;
			strcpy(to_player[i].card_name[j], deck.card_name[aux]);
			printf("Player %d - Card %d == %s\n", i, j, to_player[i].card_name[j]);
		}
		printf("\n");
		serialize_cards(truco.player[i].sock_fd, to_player[i]);
	}
}

void* recv_client(void* arg) {
	Player* player = (Player*) arg;

	while(1){
		Play play = deserialize_play(player->sock_fd);

		printf("RECEBI COISA\n");

		pthread_mutex_lock(&mutex);
		add_queue(queue, play);
		pthread_mutex_unlock(&mutex);

	}

	return NULL;
}

Play parse_client(Play* play) {

	// Convert the user input to integer to see if it is a play.
	int card_id = strtol(play->msg, NULL, 10);
	play->card_id = card_id;

	if (verify_play(truco.player[play->player], card_id)) {
		if(verify_turn(*play))
			play->flag = 'b';
		else
			play->flag = 'z';
		return *play;
	}

	//	Case player asks for 'Truco'
	if (strcmp(play->msg, "TRUCO") == 0) {
		play->flag = 't';
		return *play;
	}

	//	Otherwise will be a chat message
	play->flag = 'a';

	return *play;
}

void control(Play *play) {
	const char protocol = play->flag;

	switch(protocol) {
		case 'a':
			chat_msg(*play);
		break;

		case 'b': {
			play_card(&truco, play->card_id, play->player);
			if(check_table(&truco))
				play->flag = 'c';
			else
				play->flag = 'd';
			control(play);
		}
		break;

		case 'c': {
			// Here the round is over and the point count is decided
			const int winner = round_winner(truco.table);
			manages_score(winner);
		}
		break;

		case 'd':{
			//	Here is just a new turn
			new_turn(play);
		}
		break;

		case 't':
			//	TRUCO BET
		break;

		case 'z':
			// NOTHING FOR NOW
		break;

		default:
			error("Error in the server control!");
		break;
	}
}



int verify_turn(Play play){
		return play.player == turn;
}

void chat_msg(Play play){
	int i;

	for(i = 0; i < NUM_PLAYERS; ++i) {
		if (truco.player[i].id != play.player) {
			serialize_play(truco.player[i].sock_fd, play);
		}
	}
}

void new_turn(Play* play){
	int i;
	printf("\n\n ======Print client data=====\n\n");
	// Update turn global variable.
	printf("TURNO ANTES: %d\n", turn);
	turn++;
	turn %= NUM_PLAYERS;
	printf("TURNO DEPOIS: %d\n", turn);

	//	Flag to warn the new turn for players
	play->flag = turn - 0;
	strcpy(play->msg, deck.card_name[play->card_id]);

	for(i = 0; i < NUM_PLAYERS; ++i) {
		serialize_play(truco.player[i].sock_fd, *play);
	}


	printf("Player: %d\n", play->player);
	printf("Card ID: %d\n", play->card_id);
	printf("Message: %s\n", play->msg);
	printf("Flag: %c\n", play->flag);
	printf("Flag: %d\n", play->flag);
	printf("\n\n ============================\n\n");

}

void new_round(Play* play){
	play->flag = 1;
}

void manages_score(const int winner){
	if(winner != INVALID){
		score[winner] += round_value;
		printf("JOGADOR %d GANHOU!!.\n", winner);
	} else {
		printf("EMPATE\n");
	}
}

int check_table(Game *game) {
	return game->on_table == NUM_PLAYERS * HAND;
}

// ===========================================================================================================
// ===========================================================================================================

void init_deck() {

	strcpy(deck.card_name[0], "4 de Ouros");
	strcpy(deck.card_name[1], "4 de Espadas");
	strcpy(deck.card_name[2], "4 de Copas");

	strcpy(deck.card_name[3], "5 de Ouros");
	strcpy(deck.card_name[4], "5 de Espadas");
	strcpy(deck.card_name[5], "5 de Copas");
	strcpy(deck.card_name[6], "5 de Paus");

	strcpy(deck.card_name[7], "6 de Ouros");
	strcpy(deck.card_name[8], "6 de Espadas");
	strcpy(deck.card_name[9], "6 de Copas");
	strcpy(deck.card_name[10], "6 de Paus");

	strcpy(deck.card_name[11], "7 de Espadas");
	strcpy(deck.card_name[12], "7 de Paus");

	strcpy(deck.card_name[13], "Q de Ouros");
	strcpy(deck.card_name[14], "Q de Espadas");
	strcpy(deck.card_name[15], "Q de Copas");
	strcpy(deck.card_name[16], "Q de Paus");

	strcpy(deck.card_name[17], "J de Ouros");
	strcpy(deck.card_name[18], "J de Espadas");
	strcpy(deck.card_name[19], "J de Copas");
	strcpy(deck.card_name[20], "J de Paus");

	strcpy(deck.card_name[21], "K de Ouros");
	strcpy(deck.card_name[22], "K de Espadas");
	strcpy(deck.card_name[23], "K de Copas");
	strcpy(deck.card_name[24], "K de Paus");

	strcpy(deck.card_name[25], "Ás de Ouros");
	strcpy(deck.card_name[26], "Ás de Copas");
	strcpy(deck.card_name[27], "Ás de Paus");

	strcpy(deck.card_name[28], "2 de Ouros");
	strcpy(deck.card_name[29], "2 de Espadas");
	strcpy(deck.card_name[30], "2 de Copas");
	strcpy(deck.card_name[31], "2 de Paus");

	strcpy(deck.card_name[32], "3 de Ouros");
	strcpy(deck.card_name[33], "3 de Espadas");
	strcpy(deck.card_name[34], "3 de Copas");
	strcpy(deck.card_name[35], "3 de Paus");

	// Manilhas
	strcpy(deck.card_name[36], "7 de Ouros");
	strcpy(deck.card_name[37], "Ás de Espadas");
	strcpy(deck.card_name[38], "7 de Copas");
	strcpy(deck.card_name[39], "4 de Paus");

	// Tier 0
	deck.card_value[0] = 0;
	deck.card_value[1] = 0;
	deck.card_value[2] = 0;

	// Tier 1
	deck.card_value[3] = 1;
	deck.card_value[4] = 1;
	deck.card_value[5] = 1;
	deck.card_value[6] = 1;

	// Tier 2
	deck.card_value[7] = 2;
	deck.card_value[8] = 2;
	deck.card_value[9] = 2;
	deck.card_value[10] = 2;

	// Tier 3
	deck.card_value[11] = 3;
	deck.card_value[12] = 3;

	// Tier 4
	deck.card_value[13] = 4;
	deck.card_value[14] = 4;
	deck.card_value[15] = 4;
	deck.card_value[16] = 4;

	// Tier 5
	deck.card_value[17] = 5;
	deck.card_value[18] = 5;
	deck.card_value[19] = 5;
	deck.card_value[20] = 5;

	// Tier 6
	deck.card_value[21] = 6;
	deck.card_value[22] = 6;
	deck.card_value[23] = 6;
	deck.card_value[24] = 6;

	// Tier 7
	deck.card_value[25] = 7;
	deck.card_value[26] = 7;
	deck.card_value[27] = 7;

	// Tier 8
	deck.card_value[28] = 8;
	deck.card_value[29] = 8;
	deck.card_value[30] = 8;
	deck.card_value[31] = 8;

	// Tier 9
	deck.card_value[32] = 9;
	deck.card_value[33] = 9;
	deck.card_value[34] = 9;
	deck.card_value[35] = 9;

	// Manilhas
	deck.card_value[36] = 10;
	deck.card_value[37] = 11;
	deck.card_value[38] = 12;
	deck.card_value[39] = 13;
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

void test_shuffle(Game* truco) {
	shuffling_deck(truco);

	int i, j;
	for(i = 0; i < NUM_PLAYERS; ++i){
		for(j = 0; j < HAND; ++j){
			printf("Player %d - Card %d == %d\n", i, j, truco->player[i].card_id[j]);
		}
		printf("\n");
	}
}

/*
Play* parse_msg(Play play) {

	int i;

	if(play.msg[0] == ':') {
		for(i = 0; i < NUM_PLAYERS; ++i) {
			if (truco.player[i].id != play.player) {
				serialize_play(truco.player[i].sock_fd, play);
				return NULL;
			}
		}
	} else {
		if(play.player == turn) {
			printf("É seu turno\n");
			if (play_card(&truco, play.card_id, play.player)) {
				// VALID PLAY
				if(truco.on_table == NUM_PLAYERS) {
					// IMPLEMENT TRUCO LOGIC HERE LATER
					int tmp = round_winner(truco.table);
					if(tmp != INVALID)
						score[tmp] += round_value;

					turn = 0;
					truco.on_table = 0;
					round_value = 2;
				} else {
				++truco.on_table;
				++turn;

				}
			}
		}else {
			printf("Não é seu turno\n");
		}
	}

	return NULL;
}
*/
