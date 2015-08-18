#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#define MAX_LEN_MSG 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_header.h"
#include "../global.h"

int fd, cur_status;
char user_name[MAX_NAME_LENGTH];

enum status {
	LOGIN,
	SEL_TABLES,
	GAME,
	DEAD
};

struct msg {
	int type;
	int len;
};

void create_msg(int type, int len, void *data, unsigned char *buf);
void logicHandlerLogin(int type, void *buf);
int logicEventLogin(char *login, char *pass, int registerFlag);
void run(char *ip, char *namePort);
void create_msg(int type, int len, void *data, unsigned char *buf);


#endif