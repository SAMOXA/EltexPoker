#include "start.h"

void startGameServer(int pipe, int tableId, int port) {
	init_game_server_network(port, pipe);
	initGameLogic();
	game_server_loop();
	exit(0);
}
