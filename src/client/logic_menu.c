#include "logic.h"
#include "../graf/graf_api.h"


void create_msg(int type, int len, void *data, unsigned char *buf) {
	struct msg *msg;
	//unsigned char *buf = (unsigned char *) malloc(sizeof(struct msg) + len);
	msg = (struct msg *) buf;

	msg->type = type;
	msg->len = len;
	memcpy(buf + sizeof(struct msg), data, len);
}


void logicExitMenu() {
	net_disconnect_server(fd);
	exit(1);
}

/* Обработка ответов от сервера
*/
int logicHandlerBegin(int type) {
	unsigned char buf[MAX_LEN_MSG];
	int flg, type_recv, len;

	flg = net_receive(fd, buf, &type_recv, &len);
	if (flg == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	if (type != type_recv) {
		printf("Несоответствие типов сообщений\n");
		logicExitMenu();
	}
	switch(type) {
		/* Обработка авторизации 
		*/
		case LOG_IN:
		case REGISTRATION:
		{
			struct loginResponce_t *logResp;
			logResp = (struct loginResponce_t *) buf;

			if (logResp->status == STATUS_OK) {
				if (type == REGISTRATION) {
					printf("Registration");
					cur_status = LOGIN;
				}
				else {
					printf("Authorization");
					cur_status = SEL_TABLES;
				}
				printf(" is succesfull\n");
			} else {
				printf("%s\n", logResp->errorBuf);
			}
			break;
		}
		/* Обработка получения списка столов
		*/
		case LIST_TABLE: 
		{
			if (len == 0 ) {
				printf("List table is empty");
				return 0;
			}
			int n = len / sizeof(struct table_t), i, j;
			struct table_t *table = (struct table_t *) buf;

			for(i = 0; i < n; i++) {
				if (table->id == -1) continue;
				printf("\nid = %d\n", table->id);
				for (j = 0; j < MAX_PLAYERS_PER_TABLE; j++) {
					printf("%d) %s\n", j + 1, table->tables[j]);
					table++;
				}
			}

			break;
		} 
		/*Обработка подключения и создания стола */
		case CREATE_TABLE:
		case CONNECT_TO_TABLE:
		{
			struct selectResponce_t *selResp = (struct selectResponce_t *) buf;
			if( selResp->status == STATUS_OK ) {
				if (type == CREATE_TABLE) 
					printf("Table is created\n");
				else
					printf("Succesfull connection to table\n");
				cur_status = GAME;
				printf("port = %d\n", selResp->port);
				printf("session = %d\n", selResp->session);
			} else {
				printf("%s\n", selResp->error);
			}
			break;
		}	
	}
	return 0;
}   

/* Запрос авторизации
*/
int logicEventLogin(char *login, char *pass, int type) {
	int flg;
	struct loginRequest_t logReq;
	/*
	if (strlen(login) >= MAX_NAME_LENGTH || strlen(pass) >= MAX_NAME_LENGTH) {
		//grafDrawText("Превышена длина имени или пароля\n");
		logicExitMenu();
	}*/
	strcpy(user_name, login);

	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	
	flg = net_send(fd, &logReq, type, sizeof(struct loginRequest_t));
	if (flg == -1) {
		printf("Connection problem\n");
		//graf_draw_text("Connection problem");
		logicExitMenu();
	}
	else {
		return logicHandlerBegin(type);
	}
	return 0;
}

/* Запрос на подключение или создание стола
*/
int logicEventTable(int id, int flg) {
	int err = 0;
	struct selectRequest_t selReq;
	strcpy(selReq.name, user_name);
	selReq.tableID = id;
	err = net_send(fd, (void *)&selReq, flg, sizeof(selReq));
	if (err == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	return logicHandlerBegin(flg);
}

/* Запрос списка столов
*/
int logicGetTableList() {
	int err = 0;
	err = net_send(fd, NULL, LIST_TABLE, 0);	
	if (err == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	return logicHandlerBegin(LIST_TABLE);
}

/*Запрос создания стола
*/
void loginCreateTable() {
	logicEventTable(-1, CREATE_TABLE);
}

/*Запрос подключения к столу
*/
void loginConnectTable(int id) {
	logicEventTable(id, CONNECT_TO_TABLE);
}

/*Запрос обновления столов
*/
void logicRefreshTables() {
	logicGetTableList();
}


int logicSelTable() {
	//get_tables()
	int tmp, ret = 0;
	printf("\n1) CREATE TABLE\n2) CONNECT TO TABLE\n");
	scanf("%d", &tmp);
	switch(tmp) {
		case 1:
		{
			loginCreateTable();
			break;
		}
		case 2:
		{
			printf("Input id table:\n");
			scanf("%d", &tmp);
			loginConnectTable(tmp);
			break;
		}
	}
	return ret;
	
}

/*void logicInitGrafAPI() {
	graf_list_exit_event = logicExitMenu;
}*/


void logicGame() {

}



void run(char *ip, char *namePort) {
	int port = atoi(namePort), stop = 0;

	fd = net_create_connect_server(ip, port);
	if (fd < 0) {
		printf("Server unavaible\n");
		exit(1);
	}
	cur_status = LOGIN;
	while(stop != 1) {
		switch(cur_status) {
			/* Авторизация
			*/
			case LOGIN:		
			{
				//graf_draw_logging();
				char flg; 
				int registerFlag;
				char login[MAX_NAME_LENGTH], pass[MAX_NAME_LENGTH];
				printf("Create new profile?(y/n)\n");
				scanf("%s", &flg);
				if (flg == 'y') registerFlag = REGISTRATION;
				else registerFlag = LOG_IN; 
				printf("Input login:\n");
				scanf("%s", login);
				printf("Input pass:\n");
				scanf("%s", pass);
				if (logicEventLogin(login, pass, registerFlag) == -1) cur_status = DEAD;
				break;
			}	
			/* Выборка столов
			*/
			case SEL_TABLES:
			{
				if (logicGetTableList() == -1 || logicSelTable() == -1) 
					cur_status = DEAD;
				cur_status = DEAD;
				break;
			}	
			/* Стадия игры
			*/
			case GAME:
			{
				break;
			}
			/* Конец
			*/
			case DEAD: 
			{
				net_disconnect_server(fd);
				stop = 1;
				break;
			}
		}
	}
}