#include "../global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"
#include "net_implementation.h"


struct msg {
	int type;
	int len;
};

enum status {
	LOGIN,
	SEL_TABLES,
	GAME,
	DEAD
};

enum initAction {
	REGISTRATION,
	LOG_IN,
	CREATE_TABLE,
	CONNECT_OF_TABLE,
	LIST_TABLE
};

int fd, cur_status;

void create_msg(int type, int len, void *data, unsigned char *buf) {
	struct msg *msg;
	//unsigned char *buf = (unsigned char *) malloc(sizeof(struct msg) + len);
	msg = (struct msg *) buf;

	msg->type = type;
	msg->len = len;
	memcpy(buf + sizeof(struct msg), data, len);
}

int logicSend(int fd, int type, void* data, int len) {
	unsigned char buf[MAX_LEN_MSG];
	create_msg(type, len, data, buf);
	return net_send(fd, buf, len + sizeof(struct msg));
}

int logicRecv(int fd, void *buf, int len) {
	return net_receive(fd, buf, len);
}


int logicEventLogin(char *login, char *pass, char registerFlag) {
	int flg;
	unsigned char buf[MAX_LEN_MSG];
	struct loginRequest_t logReq;
	if (strlen(login) >= MAX_NAME_LENGTH || strlen(pass) >= MAX_NAME_LENGTH) {
		//grafDrawText("Превышена длина имени или пароля\n");
		return -1;
	}
	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	logReq.registerFlag = registerFlag;
	flg = logicSend(fd, LOG_IN, (void *) &logReq, sizeof(logReq));
	if (flg == -1) {
		//graf_draw_text("Проблемы с соединением");
		return -1;
	}
	else {
		logicRecv(fd, buf, sizeof(struct msg) + sizeof);
	}
}

   


void* listen(void *arg) {
}

void* sender(void *arg) {
	
}

void run(char *ip, char *namePort) {
	int port = atoi(namePort);
	unsigned char buf_msg[MAX_LEN_MSG];

	fd = net_connect_server(ip, port);
	if (fd < 0) {
		printf("Server unavaible\n");
		exit(1);
	}
	cur_status = LOGIN;
	while(cur_status != DEAD) {
		switch(cur_status) {
			case LOGIN:		
			{
				//graf_draw_logging();
				//logicEventLogin(char *login, char *pass, char registerFlag)

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