#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../global.h"


#define SIZE_DATA MAX_TABLES_COUNT * 4

struct dataPlayers {
	char name[MAX_NAME_LENGTH];
	char pswd[MAX_PASS_LENGTH];
} data[SIZE_DATA];

static int readFile();
static int checkName(char *name);
static int checkPasswd(int numCheck, char *pswd);
static void saveFile();
static void registration(void * buf);
static void login(void *buf);
static void tableList();


#endif //LOGIC_H