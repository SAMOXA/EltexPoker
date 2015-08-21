#include "logic.h"


int graf;

struct graf_list_t graf_list;
pthread_cond_t cond;
pthread_mutex_t mut;



void logicExitMenu() {
	net_disconnect_server(fd);
	if (graf == 1) grafExitList();
	printf("EXIT\n");
	exit(0);
}

void logicHandlerTable(struct table_t *tables, struct graf_list_t *graf_list) {
	int i, j;

	for(i = 0; i < MAX_TABLES_COUNT; i++) {
		graf_list->tables[i].enabled = (tables[i].id == 0) ? 0 : 1;
		graf_list->tables[i].id = tables[i].id;
		graf_list->tables[i].players_count = 0;
		if (tables[i].id != 0) {
			for( j = 0; j < MAX_PLAYERS_PER_TABLE; j++) {
				if (strlen(tables[i].players[j]) != 0) 
					++(graf_list->tables[i].players_count);

			}
		}
	}

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
		//case 
		case LIST_TABLE: 
		{
			struct room_t *room = (struct room_t *)buf;
			if (room->status == ROOM_STATUS_EMPTY) {
				//printf("List table is empty");
				grafDrawMsgList("List tables is empty");
				grafDrawTableList(NULL);
				break;
			}
			//int n = len / sizeof(struct table_t), i, j;
			//struct table_t *table = (struct table_t *) buf;
			//strcpy(graf_list.title, "Hello");
		/*	if (sizeof(graf_list.tables) < len){
				printf("Не хватает памяти под  столы\n");
				logicExitMenu();
			}*/
			memcpy(graf_list.tables, room->tables, sizeof(graf_list.tables));
			logicHandlerTable(room->tables, &graf_list);			
			grafDrawTableList(&graf_list);

/*			for(i = 0; i < n; i++) {
				if (table->id == -1) continue;
				printf("\nid = %d\n", table->id);
				for (j = 0; j < MAX_PLAYERS_PER_TABLE; j++) {
					printf("%d) %s\n", j + 1, table->tables[j]);
					table++;
				}
			}*/

			break;
		} 
		/*Обработка подключения и создания стола */
		case CREATE_TABLE:
		case CONNECT_TO_TABLE:
		{
			struct selectResponce_t *selResp = (struct selectResponce_t *) buf;
			if( selResp->status == STATUS_OK ) {
				if (type == CREATE_TABLE) 
					//printf("Table is created\n");
					grafDrawMsgList("Table is created");
				else
					grafDrawMsgList("Succesfull connection to table");
				sleep(3);
				cur_status = GAME;
				/*grafDrawMsgList("port = %d\n", selResp->port);
				grafDrawMsgList("session = %d\n", selResp->session);*/

				/* Нужно потом это заменить */
				port = selResp->port;
				session = selResp->session;
				logicExitMenu();
			} else {
				grafDrawMsgList(selResp->error);
				cur_status = DEAD;
				logicExitMenu();
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

void logicInitGrafList() {
	graf_list_exit_event = logicExitMenu;
	graf_list_select_event = loginConnectTable;
	graf_list_create_event = loginCreateTable;
	graf_list_refresh_event = logicRefreshTables;
}


int logicSelTable() {
	logicInitGrafList();
	grafInitList();
	graf = 1;
	if( logicGetTableList() == -1 ) {
		printf("Error: logicGetTableList()\n");
		exit(1);
	}
	while(1){
		if(cur_status != SEL_TABLES){
			break;
		}
		usleep(100);
	}
	printf("End SEL_TABLES");
	//get_tables()
	/*int tmp, ret = 0;
	if( logicGetTableList() == -1 )
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
	}*/
	return 0;
	
}

void handler_sig(int sig) {
	printf("dddddd\n");
	logicExitMenu();
}


void run(char *ip, char *namePort) {
	signal(SIGINT, handler_sig);
	signal(SIGTERM, handler_sig);
	graf = 0;
	int port_ = atoi(namePort), stop = 0;

	fd = net_create_connect_server(ip, port_);
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
				if (logicSelTable() == -1) 
					cur_status = DEAD;
				cur_status = DEAD;

				break;
			}	
			/* Стадия игры
			*/
			case GAME:
			{
				//run_game(ip, port, session);
				break;
			}
			/* Конец
			*/
			case DEAD: 
			{
				logicExitMenu();
				stop = 1;
				break;
			}
		}
	}
}