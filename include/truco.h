#ifndef TRUCO_H
#define TRUCO_H

#include "utils.h"
/*

Manilhas - São fixas. Dá mais forte para a mais fraca: 4 de Paus/7 de Copas/Ás de Espadas/7 de Ouros
Rodada - uma sequência de 4 jogadas, onde cada jogador joga uma carta.
Mão - composta de duas a três rodadas, e vale inicialmente 2 pontos.
Truco - proposta para subir o valor de pontos da mão para 4.
Seis - proposta para subir o valor de pontos da mão para 6.
Dez - proposta para subir o valor de pontos da mão para 10.
Doze - proposta para subir o valor de pontos da mão para 12.
Mão de 10 - quando uma das duplas está com 10 pontos.
Mão de ferro - quando as duas duplas estão com 10 pontos.
Carta coberta - carta jogada virada (com a face voltada para baixo), que não vale nada.
Ordem das cartas (da menor para maior): 4, 5, 6, 7, Q, J, K, A, 2, 3

Dá mais forte para a mais fraca:

Manilhas
-----------
4 de Paus == 39
7 de Copas == 38
Ás de Espadas == 37
7 de Ouros == 36

Tier 9
-----------
3 de Paus == 35
3 de Copas == 34
3 de Espadas == 33
3 de Ouros == 32

Tier 8
-----------
2 de Paus == 31
2 de Copas == 30
2 de Espadas == 29
2 de Ouros == 28

Tier 7
-----------
Ás de Paus == 27
Ás de Copas == 26
Ás de Ouros == 25

Tier 6
-----------
K de Paus == 24
K de Copas == 23
K de Espadas == 22
K de Ouros == 21

Tier 5
-----------
J de Paus == 20
J de Copas == 19
J de Espadas == 18
J de Ouros == 17

Tier 4
-----------
Q de Paus == 16
Q de Copas == 15
Q de Espadas == 14
Q de Ouros == 13

Tier 3
-----------
7 de Paus == 12
7 de Espadas == 11

Tier 2
-----------
6 de Paus == 10
6 de Copas == 09
6 de Espadas == 08
6 de Ouros == 07

Tier 1
-----------
5 de Paus == 06
5 de Copas == 05
5 de Espadas == 04
5 de Ouros == 03

Tier 0
-----------
4 de Copas == 02
4 de Espadas == 01
4 de Ouros == 00
*/

struct Deck {
	char card_name[NUM_CARDS][MSG_SIZE];
	int32_t card_value[NUM_CARDS];
} deck;

typedef struct {
	int32_t id;
	int32_t sock_fd;
	int32_t card_id[HAND];
} Player;

typedef struct {
	Player player[NUM_PLAYERS];
	int32_t table[NUM_PLAYERS];
	int32_t on_table;
}Game;

int round_winner(int* table);
void play_card(Game* game, const int32_t card_id, const int32_t player_id);
void shuffling_deck(Game* p);
int verify_play(Player player, int card_id);

#endif /* !TRUCO_H */
