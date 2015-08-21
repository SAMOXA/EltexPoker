#include "logic.h"
#ifdef HAVE_NCURSES

#include "graf_api.h"
struct graf_list_t graf_list;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#endif

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

#ifndef HAVE_NCURSES
int selectTable(){
#else
void selectTable() {
	int playersCount;
#endif
	struct table_t *tables;
	int msgType;
	int i, j;
	int id;

	sendMsg(LIST_TABLE, 0, NULL);
	tables = (struct table_t *)recvMsg(&msgType);
#ifndef HAVE_NCURSES
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
#else
	memset(&graf_list, 0, sizeof(struct graf_list_t));
	sprintf(graf_list.title, "%s", "Tables");
	if(msgType == LIST_TABLE){
		for(i=0;i<MAX_TABLES_COUNT;i++){
			graf_list.tables[i].id = tables[i].id;
			graf_list.tables[i].enabled = (tables[i].id == -1);
			playersCount++;
			for(j=0;j<MAX_PLAYERS_PER_TABLE;j++){
				if(tables[i].players[j][0] != 0){
					playersCount++;
				}
			}
			graf_list.tables[i].players_count = playersCount;
		}
	}
	grafDrawTableList(&graf_list);
	return;
#endif
}

#ifndef HAVE_NCURSES
int createTable(){
#else
void createTable() {
#endif
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
#ifndef HAVE_NCURSES
		return 0;
#else
		grafDrawMsgList("unclock");
		pthread_mutex_unlock(&lock);
		return;
#endif
	}
#ifndef HAVE_NCURSES
	printf(selectResp->error);
	return -1;
#else
	grafDrawMsgList(selectResp->error);
	return;
#endif
}

#ifndef HAVE_NCURSES
int connectToTable(int id) {
#else
void connectToTable(int id) {
#endif
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
#ifndef HAVE_NCURSES
		return 0;
#else
		pthread_mutex_unlock(&lock);
		return;
#endif
	}
#ifndef HAVE_NCURSES
	printf(selectResp->error);
	return -1;
#else
	grafDrawMsgList(selectResp->error);
	return;
#endif
}

#ifdef HAVE_NCURSES
void startGraphicsWaitLoop() {
	pthread_mutex_lock(&lock);
}
#endif

int lobbyServer(){
	int retCode;
	char flg;
	int tableId;
	retCode = initNetwork();

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
#ifndef HAVE_NCURSES
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
#else
	logicInitGrafList();
	grafInitList();
	selectTable();
	pthread_mutex_lock(&lock);
	startGraphicsWaitLoop();
#endif
	networkDisconnect();
#ifdef HAVE_NCURSES
	grafExitList();
#endif
	return 0;
}

#ifdef HAVE_NCURSES
void exitCallback() {
	grafExitList();
	networkDisconnect();
	exit(1);
}

void logicInitGrafList() {
	graf_list_exit_event = exitCallback;
	graf_list_select_event = connectToTable;
	graf_list_create_event = createTable;
	graf_list_refresh_event = selectTable;
}
#endif
