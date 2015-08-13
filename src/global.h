#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LENGTH 10
#define MAX_PASS_LENGTH 16
#define MAX_PLAYERS_PER_TABLE 4
#define MAX_TABLES_COUNT 5
#define SESSION_TOKEN_LENGTH 16

struct loginRequest_t {
	char name[MAX_NAME_LENGTH];
	char pass[MAX_PASS_LENGTH];
	char registerFlag;
};

struct table_t {
	char id;
	unsigned char [MAX_PLAYERS_PER_TABLE][MAX_NAME_LENGTH];
};

struct loginResponce_t {
	char status;
	int startCounter;
	struct table_t tables[MAX_TABLES_COUNT];
};

struct selectRequest_t {
	char token; //MD5(pass+login+counter)
	char selectedId;
};

struct selectResponce_t {
	int port;
}

#endif //GLOBAL_H
