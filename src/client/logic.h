#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#define MAX_LEN_MSG 1000

int fd, cur_status;

enum status {
	LOGIN,
	SEL_TABLES,
	GAME,
	DEAD
};

struct msg {
	int type;
	int len;
};

void create_msg(int type, int len, void *data, unsigned char *buf);
void logicHandlerLogin(void *buf, int type);
int logicEventLogin(char *login, char *pass, int registerFlag);
void run(char *ip, char *namePort);
void create_msg(int type, int len, void *data, unsigned char *buf);


#endif