#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"

typedef struct {
	int32_t id;
	int32_t socket;
	int32_t card[HAND];
}My;

typedef struct{
	int32_t card[NUM_PLAYERS];
	int32_t score[NUM_PLAYERS];
	int32_t round_score[NUM_PLAYERS];

	int32_t	flag;																	//	Flag to sinalize that the current hand is over
	int32_t num_ctable;														//	Number of card at the table
	int32_t turn;																	//	Current turn, match the player identification
	int32_t round_value;													//	How much this round costs
}Table;

struct Deck {
	char card_name[NUM_CARDS][CARD_NAME_SIZE];
	int32_t card_value[NUM_CARDS];
} deck;


void init_deck();
void init_hand(My* my, Table* table);
void init_round(My* my, Table* table);

void recv_hand (My* my);
void recv_round_winner(My* my, Table* table);
void recv_hand_winner(My* my, Table* table);

void new_turn(Table* table);
void controller(My* my, Table* table, const int32_t control);
void update_table(Table* table, int32_t card_id);
int32_t user_input ();
int32_t is_gameover(int32_t* table);

void show_score(Table table);
void show_round_score (Table table);
void show_table_cards (Table table);
void show_cards (My my);

void setup_client(const int port_number, int* socket_fd, struct sockaddr_in* server_addr, struct hostent* server);
#endif // !CLIENT_H
