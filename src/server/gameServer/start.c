#include "start.h"

void startGameServer(int pipeFd, int tableId, int port) {
	init_game_server_network(port, pipeFd);
	initGameLogic();
	game_server_loop();
	exit(0);
}
