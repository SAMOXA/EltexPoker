#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include "../global.h"
#include "server_network.h"
#include "internalIPC.h"

#define SIZE_DATA (MAX_TABLES_COUNT * MAX_PLAYERS_PER_TABLE) /*Максимально кол-во данных по игрокам*/ 

enum statusTable {EMPTY, SLEEP, FULL, PLAY}; /*Состояние для столов и портов*/

/*Информация о игроках*/
struct dataPlayers {
	char name[MAX_NAME_LENGTH];
	char pswd[MAX_PASS_LENGTH];
};
/*Информация о столах*/
struct infoOfTable {
	int countPlayer;
	int status;
	int port;
};
/*Таблица Имя - id*/
struct listNameId {
	char name[MAX_NAME_LENGTH];
	int id;
};
/*Информация по портам*/
struct gamePort {
	int listPort[MAX_TABLES_COUNT];
	int statusListPort[MAX_TABLES_COUNT];
};

/*Клиентская часть*/

/*Регистрация*/
void registration(void * buf);
/*Вход*/
void login(void *buf);
/*Получение списка столов*/
void tableList();
/*Создание нового стола*/
void createTable(void *buf);
/*Подключение к существующему столу*/
void connectTable(void *buf);

/*Внутресерверная часть*/

/*Подтверждение от игрового серве о подключение игрока, на вход id сервера*/
void confirmedConnect(void *buf, int serverID);
/*Удаление стола, на вход ID стола*/
void removeTable(int id);
/*Удаление игрока*/
void removePlayer(void *buf);
#endif //LOGIC_H