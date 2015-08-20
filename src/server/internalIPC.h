#ifndef INTERNAL_IPC_H
#define INTERNAL_IPC_H

#include "../global.h"


enum destinationType_e {
/* For listen server */
	GAME_SERVER,
	CURRENT,
/* For game server */
	LOBBY_SERVER,
	CLIENT,
	ALL_CLIENTS
};

enum messagesType {
	INTERNAL_NEW_PLAYER,
	INTERNAL_PLAYER_CONFIRMED,
	INTERNAL_PLAYER_LEFT,
	INTERNAL_PLAYERS_UPDATE,
	INTERNAL_TABLE_DELETE
};

struct newPlayer_t {
	unsigned int id;
	char name[MAX_NAME_LENGTH];
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

void startGameServer(int fd, int tableID, int port);

#endif //INTERNAL_IPC_H
