#include "events.h"


void events(int dest_type, int id, int msg_type, void *buf) {

	switch (dest_type) {
	case CURRENT: /*CURRENT*/
		/*Клиентская логика*/
		switch (msg_type) {
		case REGISTRATION: /*Регистрация*/
			registration(buf);
			break;
		case LOG_IN: /*Авторизация*/
			login(buf);
			break;
		case CREATE_TABLE: /*Создание стола*/
			createTable(buf);
			break;
		case CONNECT_TO_TABLE: /*Подключение к столу*/
			connectTable(buf);
			break;
		case LIST_TABLE: /*Список столов*/
			tableList();
			break;
		}
		break;

	case GAME_SERVER: /*GAME_SERVER*/
		switch (msg_type) {
		/*Внутресерверная логика*/
		case INTERNAL_PLAYER_CONFIRMED: /*Подтверждение подключения*/
			confirmedConnect(buf, id);
			break;
		case INTERNAL_PLAYER_LEFT: /*Удаление игроков*/
			removePlayerFromTable(buf);
			break;
		case INTERNAL_TABLE_DELETE: /*Удаление стола*/
			removeTable(id);
			break;
		}
		break;
	}
}
