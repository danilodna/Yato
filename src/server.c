#include "../include/server.h"

int32_t main(int argc, char** argv) {
	verify_args_server(argc, argv);

	// GAME VARIABLES ============================================================
	int i;
	Game truco;
	srand(time(NULL));   // should only be called once
	// ===========================================================================

	// SERVER SETUP PART =========================================================
	int socket_fd; // Socket File Descriptors to be used in the process conection
	const int port_number = atoi(argv[1]); // Port number
	socklen_t client_length; // Stores the size of the address of the client. This is needed for the accept system call.
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	setup_server(&server_addr, &socket_fd, port_number);
	printf("Server setup complete.\n");
	// ===========================================================================


	// List with connections made to the socket_fd. Max size is 5
	listen(socket_fd, 5);

	printf("Wating for players to connect...\n");
	//	In the routine bellow the program waits until a connection with the client is estabelished
	for(i = 0; i < NUM_PLAYERS; ++i) {

		//	Accepting incoming clients connections
		client_length = sizeof(client_addr);
		truco.player[i].socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
		if (truco.player[i].socket_fd < 0)
	  	error("ERROR on accept");

		//	Give the player an ID
		truco.player[i].id = i;

		printf("Jogador %d conectou-se...\n", i + 1);
	}

	printf("Todos os jogadores conectados!\n\n");
	printf("=====================================================\n\n");

	// De aos players os ID deles
	for(i = 0; i < NUM_PLAYERS; ++i){
		send_card(truco.player[i].socket_fd, truco.player[i].id);
	}

	//	Assign value to the cards
	init_deck();
	init_game(&truco);

	// GAME LOOP
	while(TRUE){
		//	Reset the setup for the beggining of each round
		init_round (&truco);

		//	ROUND LOOP
		for(i = 0; i < HAND * NUM_PLAYERS; ++i) {
			//	Communicating to the player in question that is his turn.
			send_card(truco.player[truco.turn].socket_fd, 40);

			int32_t valid_play = RECV_VALID_CARD(truco);

			//	Decides what to do with the data received
			controller(&truco, valid_play);

			new_turn(&truco);
		}

		int32_t winner = round_winner(truco.table);
		if(winner != INVALID)
			truco.score[winner] += truco.round_value;
		else
			printf("It was a draw!\n");
		printf("\n\nO vencedor foi: %d!\n", winner);

		send_winner(truco, winner);

		if(is_gameover(truco))
			break;
	}

	for(i = 0; i < NUM_PLAYERS; ++i){
		if(truco.score[i] == MAX_POINTS){
			printf("O jogador %d venceu.\n", i);
		}
	}

	printf("=====================================================\n");
	printf("\t\tJogo terminado, obrigado por jogar!\n");
	printf("=====================================================\n\n");

	//	Closing sockets
	for(i = 0; i < NUM_PLAYERS; i++)
		close(truco.player[i].socket_fd);
	close(socket_fd);
	return EXIT_SUCCESS;
}

// ==============================================================================================
// ==============================================================================================

void test_shuffle(Game* truco) {
	//shuffling_deck(truco);

	int i, j;
	for(i = 0; i < NUM_PLAYERS; ++i){
		for(j = 0; j < HAND; ++j){
			printf("Player %d - Card %d == %d --- %s\n", i, j, truco->player[i].card_id[j],
			deck.card_name[truco->player[i].card_id[j]]);
		}
		printf("\n");
	}
}

void new_turn(Game* game){
	game->turn++;
	game->turn %= NUM_PLAYERS;
}

int32_t RECV_VALID_CARD(Game game) {
	//	Receive the ID of a card from player
	int32_t recv_play = recv_card(game.player[game.turn].socket_fd);

	//	Check whether the ID received is valid or not.
	//	If no valid, continue waiting for a valid id until one is received.
	while(!verify_play(game.player[game.turn], recv_play)){
		printf("ENTREI AQUI SEU ZÉ ROELA!\n");
		send_card(game.player[game.turn].socket_fd, 40);
		recv_play = recv_card(game.player[game.turn].socket_fd);
	}

	return recv_play;
}

void init_game (Game* game) {
	int32_t i;

	game->turn = 0;
	game->round_value = 2;
	for(i = 0; i < NUM_PLAYERS; i++){
		game->score[i] = 0;
	}
}

void give_cards(Game* game) {
	int32_t i, j;

	for(i = 0; i < HAND; ++i) {
		for(j = 0; j < NUM_PLAYERS; ++j){
			send_card(game->player[j].socket_fd, game->player[j].card_id[i]);
		}
	}
}

void init_round (Game* game) {
	game->turn = 0;
	game->round_value = 2;

	shuffling_deck(game);
	test_shuffle(game);

	give_cards(game);
}

void controller (Game* game, int32_t control) {
	int32_t i;

	if(control < 40) {
		//	Play the card.
		play_card(game, control, game->player[game->turn].id);

		// Communicate to all players what card was played.
		for(i = 0; i < NUM_PLAYERS; ++i){
			send_card(game->player[i].socket_fd, control);
		}
	}

	if(control == 50) {
		// The player shout truco
	}
}

void send_winner(const Game game, const int32_t winner){
	int32_t i;
	for(i = 0; i < NUM_PLAYERS; ++i){
		send_card(game.player[i].socket_fd, 60);
		send_card(game.player[i].socket_fd, winner);
		send_card(game.player[i].socket_fd, game.round_value);
	}
}

int32_t is_gameover(Game game){
	int32_t i;

	for(i = 0; i < NUM_PLAYERS; ++i){
		if(game.score[i] == MAX_POINTS){
			printf("O jogo terminou, chapa\n");
			return TRUE;
		}
	}

	return FALSE;
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
