#include "../global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"
#include "net_header.h"


void create_msg(int type, int len, void *data, unsigned char *buf) {
	struct msg *msg;
	//unsigned char *buf = (unsigned char *) malloc(sizeof(struct msg) + len);
	msg = (struct msg *) buf;

	msg->type = type;
	msg->len = len;
	memcpy(buf + sizeof(struct msg), data, len);
}


void logicHandlerLogin(void *buf, int type) {
	printf("logicHandlerLogin()\n");
	printf("type = %d\n", type);
	switch(type) {
		case LOG_IN:
			type = REGISTRATION;
		case REGISTRATION:
		{
			struct loginResponce_t *logResp;
			logResp = (struct loginResponce_t *) buf;

			if (logResp->status == STATUS_OK) {
				printf("Авторизация прошла успешна\n");
			} else {
				printf("%s\n", logResp->errorBuf);
			}
			break;
		}	
	}
}   

int logicEventLogin(char *login, char *pass, int type) {
	int flg;
	unsigned char buf[MAX_LEN_MSG] ="a";
	struct loginRequest_t logReq;
	if (strlen(login) >= MAX_NAME_LENGTH || strlen(pass) >= MAX_NAME_LENGTH) {
		//grafDrawText("Превышена длина имени или пароля\n");
		return -1;
	}
	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	
	flg = net_send(fd, &logReq, type, sizeof(struct loginRequest_t));
	if (flg == -1) {
		printf("Проблемы с соединением\n");
		//graf_draw_text("Проблемы с соединением");
		return -1;
	}
	else {
		flg = net_receive(fd, buf, &type);
		if (flg == -1) {
			printf("Проблемы с соединением\n");
			return -1;
		}
		logicHandlerLogin(buf, type);
	}
	return 0;
}



void run(char *ip, char *namePort) {
	int port = atoi(namePort);
	unsigned char buf_msg[MAX_LEN_MSG];

	fd = net_create_connect_server(ip, port);
	if (fd < 0) {
		printf("Server unavaible\n");
		exit(1);
	}
	cur_status = LOGIN;
	while(cur_status != DEAD) {
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
				printf("dddd\n");
				cur_status = DEAD;
			}	
			case SEL_TABLES:
			{

			}
			case GAME:
			{

			}
			case DEAD: 
			{
				break;
			}
		}
	}
}