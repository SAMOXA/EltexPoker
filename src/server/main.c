#include <stdio.h>
#include "server_network.h"
#include "events.h"

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Input ip port\n");
		return 0;
	}
	init_listen_server_network(atoi(argv[2]), argv[1]);
	init();
	listen_server_loop();
	return 0;
}
