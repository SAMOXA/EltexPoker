#include "logic.h"
//#include "../graf/graf_api.h"

//struct graf_list_t graf_list;
int cur_status, session;
char user_name[MAX_NAME_LENGTH];

int login(){
	struct loginRequest_t logReq;
	char login[MAX_NAME_LENGTH], pass[MAX_NAME_LENGTH];
	struct loginResponce_t *loginResponce;
	int msgType;

	printf("Input login:\n");
	scanf("%s", login);
	printf("Input pass:\n");
	scanf("%s", pass);
	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	strcpy(user_name, login);
	sendMsg(LOG_IN, sizeof(struct loginRequest_t), &logReq);
	loginResponce = (struct loginResponce_t *)recvMsg(&msgType);
	if (loginResponce->status != STATUS_OK) {
		printf("%s\n", loginResponce->errorBuf);
		return -1;
	}
	return 0;
}

int registerate(){
	struct loginRequest_t logReq;
	char login[MAX_NAME_LENGTH], pass[MAX_NAME_LENGTH];
	struct loginResponce_t *loginResponce;
	int msgType;

	printf("Input login:\n");
	scanf("%s", login);
	printf("Input pass:\n");
	scanf("%s", pass);
	strcpy(logReq.name, login);
	strcpy(logReq.pass, pass);
	sendMsg(REGISTRATION, sizeof(struct loginRequest_t), &logReq);
	loginResponce = (struct loginResponce_t *)recvMsg(&msgType);
	if (loginResponce->status != STATUS_OK) {
		printf("%s\n", loginResponce->errorBuf);
		return -1;
	}
	return 0;
}

int selectTable(){
	struct table_t *tables;
	int msgType;
	int i, j;
	int id;

	sendMsg(LIST_TABLE, 0, NULL);
	tables = (struct table_t *)recvMsg(&msgType);
	if(msgType == EMPTY_LIST){
		printf("No tables\n");
	}
	if(msgType == LIST_TABLE){
		for(i=0;i<MAX_TABLES_COUNT;i++){
			printf("%d ", tables[i].id);
			for(j=0;j<MAX_PLAYERS_PER_TABLE;j++){
				printf("%s ", tables[i].players[j]);
			}
			printf("\n");
		}
	}
	printf("Enter table Id to connect, 0 to create new table or -1 to refresh -2 to exit\n>");
	scanf("%d", &id);
	return id;
}

int createTable(){
	struct selectRequest_t selectReq;
	struct selectResponce_t *selectResp;
	int msgType;

	strcpy(selectReq.name, user_name);
	selectReq.tableID = 0;
	sendMsg(CREATE_TABLE, sizeof(struct selectRequest_t), &selectReq);
	selectResp = (struct selectResponce_t *)recvMsg(&msgType);
	if(selectResp->status == STATUS_OK){
		port = selectResp->port;
		session = selectResp->session;
		return 0;
	}
	printf(selectResp->error);
	return -1;
}

int connectToTable(int id) {
	struct selectRequest_t selectReq;
	struct selectResponce_t *selectResp;
	int msgType;

	strcpy(selectReq.name, user_name);
	selectReq.tableID = id;
	sendMsg(CONNECT_TO_TABLE, sizeof(struct selectRequest_t), &selectReq);
	selectResp = (struct selectResponce_t *)recvMsg(&msgType);
	if(selectResp->status == STATUS_OK){
		port = selectResp->port;
		session = selectResp->session;
		return 0;
	}
	printf(selectResp->error);
	return -1;
}

int lobbyServer(){
	int retCode;
	char flg;
	int tableId;
	retCode = initNetwork();

	//graf_draw_logging();
	while(1){
		printf("Create new profile?(y/n)\n");
		scanf("%s", &flg);
		if(flg == 'y') {
			retCode = registerate();
		}else{ 
			retCode = login();
		}
		if(retCode == 0){
			break;
		}
	}
	while(1){
		tableId = selectTable();
		if(tableId == -1){
			continue;
		}
		if(tableId == 0){
			retCode = createTable();
			if(retCode < 0){
				continue;
			}
		}
		if(tableId == -2){
			exit(0);
		}
		retCode = connectToTable(tableId);
		if(retCode == 0){
			break;
		}
	}
	networkDisconnect();
	return 0;
}
