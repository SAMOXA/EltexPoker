#ifndef GAME_SERVER_START_H
#define GAME_SERVER_START_H

#include <stdlib.h>

#include "server_network.h"
#include "gameLogic.h"

void startGameServer(int pipeFd, int tableId, int port);

#endif //GAME_SERVER_START_H
