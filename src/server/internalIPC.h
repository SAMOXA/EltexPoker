#ifndef INTERNAL_IPC_H
#define INTERNAL_IPC_H

#include "global.h"

enum messagesType {
	NEW_PLAYER,
	PLAYER_CONFIRMED,
	PLAYER_LEFT,
	PLAYERS_UPDATE,
};

struct newPlayer_t {
	unsigned int id;
	unsigned int name[MAX_NAME_LENGTH];
	unsigned int money;
	unsigned int session;
};

#endif //INTERNAL_IPC_H
