#include <stdio.h>
#include "../global.h"
#include "net_implementation.h"
#include "logic.h"

/*struct in_thread {
	int fd;

};

void begin(char *ip, char *namePort) {
	int port = atoi(namePort);
	int fd;
	unsigned char buf_msg[MAX_LEN_MSG];

	fd = net_connect_server(ip, port);
	if (fd < 0) {
		printf("Server unavaible\n");
		exit(1);
	}
	

}*/

int main(int argc, char **argv){
	if (argc < 3) {
		printf("./main ip port\n");
		exit(1);
	}
	run(argv[1], argv[2]);
	return 0;
}
