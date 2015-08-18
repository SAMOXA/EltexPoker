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
	INTERNAL_PLAYERS_UPDATE,
};

struct newPlayer_t {
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	unsigned int money;
	unsigned int session;
};

void startGameServer(int fd, int tableID);

#endif //INTERNAL_IPC_H
