#include "logic.h"
#include "network.h"

int main(int argc, char **argv){
	if (argc < 3) {
		printf("./main ip port\n");
		exit(1);
	}
	while(1){
		port = atoi(argv[2]);
		strcpy(address, argv[1]);
		lobbyServer();
		gameServer();
	}
	return 0;
}
