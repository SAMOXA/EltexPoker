#include "logic.h"


struct graf_list_t graf_list;
pthread_cond_t cond;
pthread_mutex_t mut;



void logicExitMenu() {
	net_disconnect_server();
	if (graf == 1) {
		grafExitList();
		graf = 0;
	}
	pthread_cond_signal(&cond);
	printf("EXIT\n");
	exit(0);
}

/*void logicChangeStatus(int status) {
	pthread_mutex_lock(&mut);
	cur_status = status;
	pthread_mutex_unlock(&mut);
}*/

void logicExitProg() {

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

/* Ф-ия получения сообщений и обработка на ошибки
*/

int logicRecv(void *buf, int type) {
	int len, type_recv, flg = net_receive(buf, &type_recv, &len);
	if (flg == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	if (type != type_recv) {
		printf("Variance types messages\n");
		logicExitMenu();
	}
	return len;
}


/* Запрос авторизации
*/
void logicEventLogin() {
	
	struct loginRequest_t logReq;
	int type;
	char login[MAX_NAME_LENGTH], pass[MAX_PASS_LENGTH], flg;
	
	printf("Create new profile?(y/n)\n");
	scanf("%1s", &flg);
	if (flg == 'y') type = REGISTRATION;
	else type = LOG_IN; 
	printf("Input login:\n");
	scanf("%s", login);
/*	if (fgets(login, MAX_NAME_LENGTH, stdin) == NULL) {
		perror("fgets()");
		logicExitMenu();
	}*/
	printf("Input pass:\n");
	scanf("%s", pass);

	/*if (fgets(pass, MAX_PASS_LENGTH, stdin) == NULL) {
		perror("fgets()");
		logicExitMenu();
	}*/

	strcpy(user_name, login);
	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	
	flg = net_send(&logReq, type, sizeof(struct loginRequest_t));
	if (flg == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	/* Принятие ответа
	*/
	unsigned char buf[MAX_LEN_MSG];
	logicRecv(buf, type);
	struct loginResponce_t *logResp = (struct loginResponce_t *) buf;

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
}

/* Запрос на подключение или создание стола
*/
void logicEventTable(int id, int type) {

	int err = 0;
	struct selectRequest_t selReq;
	strcpy(selReq.name, user_name);
	

	selReq.tableID = id;
	err = net_send((void *)&selReq, type, sizeof(selReq));
	if (err == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	/* Принятие ответа
	*/
	unsigned char buf[MAX_LEN_MSG];
	logicRecv(buf, type);
	struct selectResponce_t *selResp = (struct selectResponce_t *) buf;
	if( selResp->status == STATUS_OK ) {
		if (type == CREATE_TABLE) {
			grafDrawMsgList("Table is created");
		}
		else {
			grafDrawMsgList("Succesfull connection to table");
		}
		sleep(2);
		
		pthread_mutex_lock(&mut);
		cur_status = GAME;
		pthread_mutex_unlock(&mut);

		port = selResp->port;
		session = selResp->session;
		logicExitMenu();
	} else {
		grafDrawMsgList(selResp->error);
		sleep(2);
		cur_status = DEAD;
		logicExitMenu();
	}
}

/* Запрос списка столов
*/
void logicGetTableList() {
	int err = 0;
	err = net_send(NULL, LIST_TABLE, 0);	
	if (err == -1) {
		printf("Connection problem\n");
		logicExitMenu();
	}
	/* Принятие ответа
	*/
	unsigned char buf[MAX_LEN_MSG];
	logicRecv(buf, LIST_TABLE);
	struct room_t *room = (struct room_t *)buf;
	if (room->status == ROOM_STATUS_EMPTY) {
		grafDrawMsgList("List tables is empty");
		grafDrawTableList(NULL);
	}
	memcpy(graf_list.tables, room->tables, sizeof(graf_list.tables));
	logicHandlerTable(room->tables, &graf_list);			
	grafDrawTableList(&graf_list);
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


void logicInitGrafList() {
	graf_list_exit_event = logicExitMenu;
	graf_list_select_event = loginConnectTable;
	graf_list_create_event = loginCreateTable;
	graf_list_refresh_event = logicGetTableList;
}


void logicSelTable() {
	logicInitGrafList();
	pthread_mutex_lock(&mut);
	grafInitList();
	graf = 1;
	logicGetTableList();
	pthread_cond_wait(&cond, &mut);	
	pthread_mutex_unlock(&mut);
}

void handler_sig(int sig) {
	printf("dddddd\n");
	if (cur_status != GAME)
		logicExitMenu();
	else {
		gameExit();
	}
}


void run(char *ip, char *namePort) {
	signal(SIGINT, handler_sig);
	signal(SIGTERM, handler_sig);
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
	graf = 0;
	int port_ = atoi(namePort), stop = 0;

	int fd = net_create_connect_server(ip, port_);
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
				logicEventLogin();
				break;
			}	
			/* Выборка столов
			*/
			case SEL_TABLES:
			{
				logicSelTable(); 
				break;
			}	
			/* Стадия игры
			*/
			case GAME:
			{
				run_game(ip, port, session);
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