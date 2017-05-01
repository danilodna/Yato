#include "../include/truco.h"

void shuffling_deck(Game* game){
	int i = 0;

	const int M = HAND * NUM_PLAYERS;
	const int N = 40;
	int in;
	int im = 0;

	unsigned char is_used[N] = { 0 }; // Flags

	// Randomize unique cards for each player;
	for (in = N - M, i =0; in < N && im < M; ++in, ++i) {
		if (i % NUM_PLAYERS == 0)
			i = 0;

		int r = rand() % in;		// Generate a random number 'r'
		if (is_used[r])
  		r = in; 							// We already have 'r'. Use 'in' instead of the generated number

		assert(!is_used[r]);
		game->player[i].card_id[im++] = r;
		(game->player[i].num_cards)++;
		is_used[r] = 1;
	}
}

int play_card(Game* game, const int32_t card_id, const int32_t player_id) {
	int i;
	for(i = 0; i < NUM_PLAYERS; ++i){
		if (game->player[player_id].card_id[i] == card_id) {
			game->table[player_id] = card_id;
			game->player[player_id].card_id[i] = -1;
			return 1;
		}
	}
	return 0;
}

int round_winner(int* card_id){
	int i, j, winner = -1;

	// O(n^2) loop that makes every comparison between cards on the table to see which one has the biggest value
	// Note that the card_id and card the value of a card(card_value) are not the same!
	// With the card_id I use the deck struct as a dictionary to search for its value.
	for(i = 0; i < NUM_PLAYERS - 1; ++i){
		for(j = i + 1; j < NUM_PLAYERS; ++j){
			if(deck.card_value[card_id[i]] > deck.card_value[card_id[j]])
				winner = i;
			if(deck.card_value[card_id[i]] < deck.card_value[card_id[j]])
				winner = j;
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
