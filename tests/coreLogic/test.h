#ifndef TEST_H
#define TEST_H

#include <string.h>

#include "global.h"
#include "gameLogic.h"
#include "win.h"

#define TEST_PLAYERS_COUNT 3

struct testStruct_t {
	unsigned char cards[5];
	unsigned char playersCard[TEST_PLAYERS_COUNT][2];
	int expectWinners[TEST_PLAYERS_COUNT];
};

#endif //TEST_H
