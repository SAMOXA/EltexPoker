#include "logic.h"

int main(int argc, char **argv){
	if (argc < 3) {
		printf("./main ip port\n");
		exit(1);
	}
	run(argv[1], argv[2]);
	return 0;
}
