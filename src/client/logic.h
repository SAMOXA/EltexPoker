#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#define MAX_LEN_MSG 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_header.h"
#include "../global.h"


/* Потом сделать (port, session) не глобальными 
*/ 
int fd, cur_status, port, session;
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

/*void create_msg(int type, int len, void *data, unsigned char *buf);
int logicEventLogin(char *login, char *pass, int registerFlag);
int logicHandlerBegin(int type);*/
void run(char *ip, char *namePort);
void run_game(char *ip, int port, int session);

//..........API
void loginCreateTable();
void loginConnectTable(int id);
void logicRefreshTables();


#endif