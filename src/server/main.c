#include <stdio.h>
#include "server_network.h"
#include "events.h"

int main(int argc, char **argv) {
	init_listen_server_network();
	init();
	listen_server_loop();
	return 0;
}
