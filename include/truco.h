#ifndef TRUCO_H
#define TRUCO_H

#include "utils.h"


struct Deck {
	char card_name[NUM_CARDS][CARD_NAME_SIZE];
	int32_t card_value[NUM_CARDS];
} deck;

typedef struct {
	int32_t id;
	int32_t socket_fd;
	int32_t card_id[HAND];
} Player;

typedef struct {
	Player player[NUM_PLAYERS];
	int32_t table[NUM_PLAYERS];
	int32_t on_table;

	int32_t score[NUM_PLAYERS];
	int32_t hand_value;
	int32_t turn;
} Game;

void shuffling_deck(Game* p);
void play_card(Game* game, const int32_t card_id, const int32_t player_id);

int32_t verify_play(Player player, int32_t card_id);

int32_t hand_winner(int32_t* score); //	Function that returns the iD of the winner of that hand
int32_t round_winner(int32_t* table); // Function that returns the ID of the winner of that round

#endif /* !TRUCO_H */
