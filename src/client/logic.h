#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#define MAX_LEN_MSG 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_header.h"
#include "../global.h"
#include <unistd.h>
#include "../graf/graf_api.h"
#include <pthread.h>

#define numCardsPlayer 2
#define numCardsTable 5

/* Потом сделать (port, session) не глобальными 
*/ 
int cur_status, port, session, graf; 
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

void gameExit();

void run(char *ip, char *namePort);
void run_game(char *ip, int port, int session);

//..........API
void loginCreateTable();
void loginConnectTable(int id);
void logicRefreshTables();


#endif