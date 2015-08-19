#ifndef INTERNAL_IPC_H
#define INTERNAL_IPC_H

#include "../global.h"

enum sourceType_e {
	SERVER,
	CLIENT,
	CURRENT,
	ALL_CLIENTS
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

struct updatePlayer_t {
	unsigned int id;
	unsigned int money;
	char win;
};

struct updatePlayers_t {
	struct updatePlayer_t updates[MAX_PLAYERS_PER_TABLE];
};


#endif //INTERNAL_IPC_H
