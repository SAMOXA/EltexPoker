#include "logic.h"


enum trans {SEND, RECV};

void create_msg(int type, int len, void *data, unsigned char *buf) {
	struct msg *msg;
	//unsigned char *buf = (unsigned char *) malloc(sizeof(struct msg) + len);
	msg = (struct msg *) buf;

	msg->type = type;
	msg->len = len;
	memcpy(buf + sizeof(struct msg), data, len);
}


int logicHandlerBegin(int type) {
	unsigned char buf[MAX_LEN_MSG];
	int flg, type_recv;

	flg = net_receive(fd, buf, &type_recv);
	if (flg == -1) {
		printf("Проблемы с соединением\n");
		return -1;
	}
	if (type != type_recv) {
		printf("Несоответствие типов сообщений\n");
		return -1;
	}
	switch(type) {
		case LOG_IN:
			type = REGISTRATION;
		case REGISTRATION:
		{
			struct loginResponce_t *logResp;
			logResp = (struct loginResponce_t *) buf;

			if (logResp->status == STATUS_OK) {
				printf("Авторизация прошла успешна\n");
				cur_status = SEL_TABLES;
			} else {
				printf("%s\n", logResp->errorBuf);
			}
			break;
		}
		case LIST_TABLE: 
		{

			break;
		} 
		case CREATE_TABLE:
		{
			struct selectResponce_t *selResp = (struct selectResponce_t *) buf;
			if( selResp->status == STATUS_OK ) {
				printf("Столешница создана\n");
				cur_status = GAME;
			} else {
				printf("%s\n", selResp->error);
			}

		}
		case CONNECT_TO_TABLE:
		{

		}	
	}
	return 0;
}   

int logicEventLogin(char *login, char *pass, int type) {
	int flg;
	struct loginRequest_t logReq;
	if (strlen(login) >= MAX_NAME_LENGTH || strlen(pass) >= MAX_NAME_LENGTH) {
		//grafDrawText("Превышена длина имени или пароля\n");
		return -1;
	}
	strcpy(user_name, login);

	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	
	flg = net_send(fd, &logReq, type, sizeof(struct loginRequest_t));
	if (flg == -1) {
		printf("Проблемы с соединением\n");
		//graf_draw_text("Проблемы с соединением");
		return -1;
	}
	else {
		return logicHandlerBegin(type);
	}
	return 0;
}

int logicCreateTable() {
	int err = 0;
	struct selectRequest_t selReq;
	strcpy(selReq.name, user_name);
	selReq.tableID = -1;
	err = net_send(fd, (void *)&selReq, CREATE_TABLE, sizeof(selReq));
	if (err == -1) printf("Проблемы с соединением\n");
	return err;
}


void logicSelTable() {
	//get_tables()
	int tmp, ret;
	printf("\n1) Создать стол\n2) Подключиться\n");
	scanf("%d", &tmp);
	switch(tmp) {
		case 1:
		{
			ret = logicCreateTable();
			tmp = CREATE_TABLE;
			break;
		}
		case 2:
		{
			break;
		}
	}
	if (ret == -1) return;
	logicHandlerBegin(tmp);
}

void logicGetTableList() {
	
}

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
			// Авторизация
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
				logicEventLogin(login, pass, registerFlag);
				break;
			}	
			case SEL_TABLES:
			{
				logicSelTable();
				cur_status = DEAD;
				break;
			}	
			case GAME:
			{
				break;
			}
			case DEAD: 
			{
				net_disconnect_server(fd);
				stop = 1;
				break;
			}
		}
	}
}