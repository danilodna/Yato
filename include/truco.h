#ifndef TRUCO_H
#define TRUCO_H

#define HAND 3
#define NUM_PLAYERS 2

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

const struct Deck {
	const char* card_name[40] = {
		"4 de Ouros",
		"4 de Espadas",
		"4 de Copas",

		"5 de Ouros",
		"5 de Espadas",
		"5 de Copas",
		"5 de Paus",

		"6 de Ouros",
		"6 de Espadas",
		"6 de Copas",
		"6 de Paus",

		"7 de Espadas",
		"7 de Paus",

		"Q de Ouros",
		"Q de Espadas",
		"Q de Copas",
		"Q de Paus",

		"J de Ouros",
		"J de Espadas",
		"J de Copas",
		"J de Paus",

		"K de Ouros",
		"K de Espadas",
		"K de Copas",
		"K de Paus",

		"Ás de Ouros",
		"Ás de Copas",
		"Ás de Paus",

		"2 de Ouros",
		"2 de Espadas",
		"2 de Copas",
		"2 de Paus",

		"3 de Ouros",
		"3 de Espadas",
		"3 de Copas",
		"3 de Paus",

		// Manilhas
		"7 de Ouros ",
		"Ás de Espadas ",
		"7 de Copas ",
		"4 de Paus"
	};
	const int32_t card_value[40] = {
		0, 0, 0,

		1, 1, 1, 1,

		2, 2, 2, 2,

		3, 3,

		4, 4, 4, 4,

		5, 5, 5, 5,

		6, 6, 6, 6,

		7, 7, 7,

		8, 8, 8, 8,

		9, 9, 9, 9,

		10, 11, 12, 13
	};
}deck;

typedef struct {
	int32_t id;
	int32_t sock_fd;
	int32_t card_id[HAND];
	int32_t num_cards;
} Player;

typedef struct {
	Player player[NUM_PLAYERS];
	int32_t table[NUM_PLAYERS];
}Game;

void shuffling_deck(Game* p);

#endif /* !TRUCO_H */
