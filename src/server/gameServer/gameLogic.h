#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "global.h"

enum gameStates {
	START,
	PRE_FLOP,
	FIRST_ROUND,
	FIRST_RISE,
	FLOP,
	ROUND,
	ROUND_RISE,
	TERN,
	RIVER,
	FINAL
};

enum playerStates {
	ACTIVE,
	PASSIVE,
	PASS,
	CONNECTING,
	WAIT,
	DISCONNECTED
};

struct player_t {
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	unsigned char state;
	unsigned int money;
	unsigned char cards[2];
	unsigned int lastBet;
};

struct gameState_t {
	unsigned int id;
	unsigned char state;
	unsigned char betPlayerIndex;
	unsigned int bet;
	unsigned char lastRisePlayerIndex;
	unsigned int lastRiseCount;
	unsigned int bank;
	unsigned char cards[5];
	player_t players[MAX_PLAYERS_PER_TABLE];
};

void newEvent();

#endif //GAMELOGIC_H
