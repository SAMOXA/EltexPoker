#include <stdio.h>
#include "global.h"
#include "internallPC.h"
#include "logic.h"

void events(int dest_type, int id, int msg_type, void *buf) {

	switch (dest_type) {
	case CLIENT:
		/*Клиентская логика*/
		switch (msg_type) {
		case REGISTRATION: /*Регистрация*/
			break;
		case LOG_IN: /*Авторизация*/

			break;
		case CREATE_TABLE: /*Создание стола*/

			break;

		case CONNECT_OF_TABLE: /*Подключение к столу*/

			break;
		case LIST_TABLE: /*Список столов*/
			break;
		}
		break;
	case PARENT:
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
