#include <stdio.h>
#include "server_network.h"
#include "events.h"
#include "check.h"

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Input ip port\n");
		return 0;
	}
	checkIP(argv[2]);
	init_listen_server_network();
	init();
	listen_server_loop();
	return 0;
}
