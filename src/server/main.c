#include <stdio.h>
#include "server_network.h"

int main(int argc, char **argv) {
	init_listen_server_network();
	listen_server_loop();
	return 0;
}
