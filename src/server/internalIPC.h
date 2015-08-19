#ifndef INTERNAL_IPC_H
#define INTERNAL_IPC_H

#include "../global.h"

enum destinationType_e {
	GAME_SERVER,
	LOBBI_SERVER,
	ALL_CLIENTS
	CLIENT,
	CURRENT
};

enum messagesType {
	INTERNAL_NEW_PLAYER,
	INTERNAL_PLAYER_CONFIRMED,
	INTERNAL_PLAYER_LEFT,
	INTERNAL_PLAYERS_UPDATE
};

struct newPlayer_t {
	unsigned int id;
	unsigned int name[MAX_NAME_LENGTH];
	unsigned int money;
	unsigned int session;
};

#endif //INTERNAL_IPC_H
