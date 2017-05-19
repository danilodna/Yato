#include "../include/client.h"

int32_t main(int argc, char** argv) {
	verify_args_client(argc, argv);

	// CLIENT VARIABLES =====================================================
	int32_t socket_fd;	// Socket File Descriptors to be used in the process conection
	struct sockaddr_in server_addr;
	struct hostent* server = gethostbyname(argv[1]);
	const int port_number = atoi(argv[2]); // Stores the port number on which the server accepts connections.
	if (server == NULL)
  	error("ERROR, no such host\n");

	setup_client(port_number, &socket_fd, &server_addr, server);

	// Estabelishing connection with server
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  	error("ERROR connecting\n");
	// ======================================================================

	init_deck();
	Table table;
	My my;

	int32_t i = 0;
	int32_t ctrl = 0;

	for(i = 0; i < NUM_PLAYERS; ++i)
		table.score[i] = 0;

	my.socket = socket_fd;

	//	First thing to receive from server is the ID of this player
	//	The ID assigned is based on the order that the client connected in the server.
	//	e.g: first connection has ID 0, second connections has ID 1... and so on.
	my.id = recv_card(my.socket);


	// GAME LOOP
	while(TRUE){

		// Receive your cards
		init_round(&my, &table);

		// LOOP to play all cards of the game
		for(i = 0; i < NUM_PLAYERS * HAND; ++i){
			//	Display the new round info.
			show_score(table);
			show_table_cards(table);
			show_cards(my);

			//	Receive flag//card
			ctrl = recv_card(socket_fd);

			//	Decide its meaning and do the correspoding action
			controller(&my, &table, ctrl);

			new_turn(&table);
		}

		controller(&my, &table, recv_card(my.socket));
	}

	// Extremely important to close the stream
	close(socket_fd);
	return EXIT_SUCCESS;
}

// =============================================================================
// =============================================================================

void new_turn(Table* table){
	table->turn++;
	table->turn %= NUM_PLAYERS;
}

void init_round(My* my, Table* table){
	int32_t i;

	for(i = 0; i < HAND; ++i){
		my->card[i] = 0;
	}

	table->num_ctable = 0;
	table->turn = 0;
	table->round_value = 2;
	for(i = 0; i < HAND * NUM_PLAYERS; ++i)
		table->card[i] = 0;

	recv_hand(my);
}

void recv_hand (My* my) {
	int32_t i;
	for(i = 0; i < HAND; ++i) {
		my->card[i] = recv_card(my->socket);
	}
}

int32_t user_input () {
	int32_t pick;
	char buffer[BUFFER_SIZE];

	printf("Entre com o número da carta que deseja jogar:\n");
	do{
		memset(buffer, 0, sizeof(buffer));
		while((getchar()) != EOF);

		//	Read the entire line
		fgets(buffer, BUFFER_SIZE, stdin);

		// Parse the value received as integer
		pick = (int32_t) strtol(buffer, NULL, 10);

		printf("\n PICK == %d\n", pick);

		if(pick < 1 || pick > HAND)
			printf("\nEntrada inválida! Tente novamente:\n");
	}while(pick < 1 || pick > HAND);

	//	Pick minus one because the index interval is [0, 2]
	//	But the numeration given to the player is [1, 3] for stylistic reasons
	return (pick - 1);
}

void controller(My* my, Table* table, const int32_t control) {
	switch (control) {
		case 40:{
			// It's your turn
			int32_t tmp = user_input();

			send_card(my->socket, my->card[tmp]);
			my->card[tmp] = -1;
			controller(my, table, recv_card(my->socket));
		}
		break;

		case 50: {
			// Someone shout TRUCO

		}
		break;

		case 60: {
			recv_winner(my, table);
		}
		break;

		default: {
			update_table(table, control);
		}
		break;
	}
}

void recv_winner(My* my, Table* table){
	int32_t winner = recv_card(my->socket);
	int32_t points = recv_card(my->socket);

	if(winner == INVALID){
		printf("FOI UM EMPATE!\n");
		return;
	}

	table->score[winner] += points;
}

void update_table(Table* table, int32_t card_id){
	table->card[table->num_ctable] = card_id;
	++table->num_ctable;
}

void show_score(Table table) {
	int32_t i;

	printf("\n\n\t\t TURNO %d \n\n", table.turn);

	printf("Placar geral:\n");
	for(i = 0; i < NUM_PLAYERS; ++i){
		printf("Jogador %d tem: %d pontos.\n", i, table.score[i]);
	}

	printf("\n");
}

void show_table_cards (Table table) {
	int32_t i;

	printf("Cartas na mesa:\n");
	for(i = 0; i < table.num_ctable; ++i){
		printf("%s\n", deck.card_name[table.card[i]]);
	}
	printf("\n");
}

void show_cards (My my) {
	int32_t i;


	for(i = 0; i < HAND; ++i){
		if(my.card[i] == INVALID)
			printf("Carta %d: JÁ JOGADA\n", i + 1);
		else
			printf("Carta %d: %s\n", i + 1, deck.card_name[my.card[i]]);
	}
	printf("\n");
}

void show_info(){

	//printf("\n\n\n\n\n\n\n");
	//printf("===================  Jogador %d  ===================\n", table.player);
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

/*
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
*/

/*
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
*/

/*
Play recv_user() {
	Play play;
	fgets(play.msg, MSG_SIZE, stdin);
	int len = strlen(play.msg);
	play.msg[--len] = '\0';
	return play;
}
*/

/*
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
*/
