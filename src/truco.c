#include "../include/truco.h"

void shuffling_deck(Game* game){
	const int cards_dist = NUM_PLAYERS * HAND;
	int in = NUM_CARDS - cards_dist;
	int i, j, im;

	unsigned char is_used[NUM_CARDS] = { 0 }; // Flags

	// Randomize unique cards for each player;
	for (i = 0, j = 0, im = 0; in < NUM_CARDS && im < cards_dist; ++in, ++im) {
		if(j % NUM_PLAYERS == 0 && j != 0){
			j = 0;
			++i;
		}

		int r = rand() % in;		// Generate a random number 'r'
		if (is_used[r])
  		r = in; 							// We already have 'r'. Use 'in' instead of the generated number

		assert(!is_used[r]);
		game->player[j].card_id[i] = r;
		++j;
		is_used[r] = 1;
	}
}

int verify_play(Player player, int card_id){
	int i;
	for(i = 0; i < HAND; ++i) {
		if(player.card_id[i] == card_id) {
			return 1;
		}
	}
	return 0;
}

void play_card(Game* game, const int32_t card_id, const int32_t player_id) {
	int i;
	for(i = 0; i < HAND; ++i) {
		if (game->player[player_id].card_id[i] == card_id) {
			game->player[player_id].card_id[i] = -1;
		}
	}
	game->table[game->on_table] = card_id;
	game->on_table++;
}

int32_t hand_winner(int32_t* score) {
	int32_t i;
	int32_t enough = 2;

	for(i = 0; i < NUM_PLAYERS; ++i){
		if(score[i] >= enough){
			return i;
		}
	}

	return -1;
}

int32_t round_winner(int32_t* table) {
	int32_t i, j, winner = -1;

	// O(n^2) loop that makes every comparison between cards on the table to see which one has the biggest value
	// Note that the card_id and card the value of a card(card_value) are not the same!
	// With the card_id I use the deck struct as a dictionary to search for its value.
	for(i = 0; i < NUM_PLAYERS - 1; ++i){
		for(j = i + 1; j < NUM_PLAYERS; ++j){
			if(deck.card_value[table[i]] > deck.card_value[table[j]])
				winner = i % NUM_PLAYERS;
			if(deck.card_value[table[i]] < deck.card_value[table[j]])
				winner = j % NUM_PLAYERS;
		}
	}

	// The return of winner indicates ID of the player who won that round.
	// 0 for the player with id 0
	// 1 for the player with id 1
	// 2 for the player with id 2
	// 3 for the player with id 3
	// -1 for draw
	return winner;
}
