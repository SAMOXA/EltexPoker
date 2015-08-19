#include <stdio.h>
#include "../global.h"
#include "internalIPC.h"
#include "logic.h"


void events(int dest_type, int id, int msg_type, void *buf) { 

	switch (dest_type) {
	case CURRENT: /*CURRENT*/
		/*Клиентская логика*/
		switch (msg_type) {
		case REGISTRATION: /*Регистрация*/
			printf("[logic] registration\n");
			registration(buf);
			printf("[logic] registration\n");

			break;
		case LOG_IN: /*Авторизация*/
			login(buf);
			printf("[logic] login\n");
			break;
		case CREATE_TABLE: /*Создание стола*/
			createTable(buf);
			printf("[logic] create table\n");
			break;

		case CONNECT_TO_TABLE: /*Подключение к столу*/
			connectTable(buf);
			printf("[logic] connect to table\n");
			break;
		case LIST_TABLE: /*Список столов*/
			tableList();
			printf("[logic] table list\n");
			break;
		}
		break;
	case GAME_SERVER: /*GAME_SERVER*/
		switch (msg_type) {
		/*Внутресерверная логика*/
		case 1: /*Подтверждение подключения*/

			break;
		case 2: /*Удаление игроков*/

			break;
		}
		break;
	}
}
