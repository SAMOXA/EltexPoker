#include "logic.h"

int flgReadFile = 0;
int countAllPlayers = 0;
int countCurrentTables = 0;
unsigned int playersID = 0;
unsigned int tableID = 0;
struct table_t lists[MAX_TABLES_COUNT]; /*Список столов*/

enum statusTable {EMPTY, SLEEP, FULL, PLAY};
int flg = 0;

struct infoOfTable {
	int countPlayer;
	int status;
	int port;
} inofList[MAX_TABLES_COUNT];

struct gamePort {
	int listPort[MAX_TABLES_COUNT];
	int statusListPort[MAX_TABLES_COUNT];
} gPorts;

int readFile() {
	FILE * fp;
	bzero(&data, sizeof(struct dataPlayers) * SIZE_DATA);
	if ((fp = fopen("data.txt", "rb")) == NULL) {
		if (system("touch data.txt") < 0 ) {
			perror("system");
		}
		return -1;
	}

	while (fscanf(fp, "%s%s", (data[countAllPlayers].name), (data[countAllPlayers].pswd)) != EOF) {
		countAllPlayers++;
	}
	fclose(fp);
	return 0;
}

int checkName(char *name) {
	int i;
	for (i = 0; i <= countAllPlayers; i++) {
		if (strcmp(name, data[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

void saveFile() {
	FILE *fp;
	int i;
	if ((fp = fopen("data.txt", "w+")) == NULL) {
	}
	for (i = 0; i <= countAllPlayers; i++) {
		fprintf(fp, "%s %s\n", data[i].name, data[i].pswd);
	}
	fclose(fp);
}

void init() {
	int i;
	bzero(&inofList, sizeof(struct infoOfTable) * MAX_TABLES_COUNT);
	gPorts.listPort[0] = LISTEN_SERVER_PORT + 1;
	gPorts.statusListPort[0] = EMPTY;
	for (i = 1; i < MAX_TABLES_COUNT; i++) {
		gPorts.listPort[i] = ++gPorts.listPort[i - 1];
		gPorts.statusListPort[i] = PLAY;
	}
}
void registration(void * buf) {

	if (flg == 0) {
		init();
		flg = 1;
	}
	struct loginRequest_t *loginReq = (struct loginRequest_t *)buf;
	struct loginResponce_t *loginRes = malloc(sizeof(struct loginResponce_t));
	bzero(loginRes, sizeof(struct loginResponce_t));

	readFile();
	if (checkName(loginReq->name) == -1) {
		strcpy(data[countAllPlayers].name, loginReq->name);
		strcpy(data[countAllPlayers].pswd, loginReq->pass);
		saveFile();
		loginRes->status = STATUS_OK;
		send_message(CURRENT, 0, REGISTRATION, sizeof(struct loginResponce_t), (void *)loginRes);
	} else {
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "Error\0");
		send_message(CURRENT, 0, REGISTRATION, sizeof(struct loginResponce_t), (void *)loginRes);
	}
	free(loginRes);
}

int checkPasswd(int numCheck, char *pswd) {
	if (strcmp(pswd, data[numCheck].pswd) == 0) {
		return 1;
	}
	return -1;
}

void login(void *buf) {
	if (flg == 0) {
		init();
		flg = 1;
	}
	int checkNum;
	struct loginResponce_t *loginRes = malloc(sizeof(struct loginResponce_t));
	struct loginRequest_t *loginReq = (struct loginRequest_t *) buf;
	if (readFile() == -1) {
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "User not found");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes); /*Некоректное имя*/
		free(loginRes);
		return;
	}
	if ((checkNum = checkName(loginReq->name)) == -1) {
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "User not found");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes); /*Пользаватель с таким именем уже есть*/
		free(loginRes);
		return;
	}
	if (checkPasswd(checkNum, loginReq->pass) == -1 ) {
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "Incorrectly password");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes); /*Некоректный пароль*/
		free(loginRes);
		return;
	} else {
		loginRes->status = STATUS_OK;
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes);
		free(loginRes);

	}
}

void tableList() {
	if (!countCurrentTables) {
		lists[0].id = -1;
		send_message(CURRENT, 0, LIST_TABLE, sizeof(struct table_t)*MAX_TABLES_COUNT, (void*) &lists);
		return;
	} else {
		send_message(CURRENT, 0, LIST_TABLE, sizeof(struct table_t)*MAX_TABLES_COUNT, (void*) &lists);
	}
}
int getNewPort () {
	int i;
	for (i = 0; i < MAX_TABLES_COUNT; i++) {
		if (gPorts.statusListPort[i] == EMPTY) {
			gPorts.statusListPort[i] = PLAY;
			return gPorts.listPort[i];
		}
	}
	return -1;
}

int getSession() {
	srand(time(NULL));
	return rand() / 10000;
}

int findEmptyTable() {
	int i;
	for ( i = 0; i < MAX_TABLES_COUNT; i++) {
		if (inofList[i].status == EMPTY) {
			return i;
		}
	}
	return -1;
}

void createTable(void *buf) {
	int pipedes[2];
	int newSessison;
	pid_t pid;
	struct newPlayer_t newPlayer;
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;

	int empt = findEmptyTable();
	if (empt == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Can not create table");
		printf("[logic]create table error\n");
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}

	++countCurrentTables;
	newSessison = getSession();
	newPlayer.session = newSessison;
	newPlayer.id = playersID++;
	newPlayer.money = 1000;
	strcpy(newPlayer.name, request->name);

	lists[empt].id = tableID++;
	strcpy(lists[empt].tables[inofList[empt].countPlayer], request->name);

	if (pipe(pipedes) < 0 ) {
		perror("pipe");
	}
	pid = fork();
	if (pid == 0) { /*Дочерний*/
		close(pipedes[1]);
		// startGameServer(pipedes[0], tableID, port);
		exit(0);
	} else {
		close(pipedes[0]);
		// if ( write(pipedes[1], (void *)&newPlayer, sizeof(struct newPlayer_t)) < 0) {
		// 	perror("write");
		// }
		responce.port = getNewPort();

		if (responce.port == -1) {
			responce.status = STATUS_BAD;
			strcpy(responce.error, "Can not create table");
			printf("[logic]create table error\n");
			send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
			return;
		}
		inofList[empt].port = responce.port;
		responce.status = STATUS_OK;
		responce.session = newSessison;
		// add_id_to_table(tableID, pipedes[1]);
		printf("create table\n");
		// send_message(GAME_SERVER, tableID, INTERNAL_NEW_PLAYER, sizeof(struct newPlayer_t), (void *) &newPlayer);
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), (void *)&responce);
		/*Close connect*/
	}
}

int checkTable(int id) {
	int i;
	for (i = 0; i < MAX_TABLES_COUNT; i++) {
		if (lists[i].id == id) {
			return i;
		}
	}
	return -1;
}

int checkFull(int num) {
	if ((inofList[num].countPlayer == MAX_PLAYERS_PER_TABLE) || (inofList[num].status == PLAY)) {
		return -1;
	}
	return 1;
}

void connectTable(void *buf) {
	struct newPlayer_t newPlayer;
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;
	int check;
	if ( (check = checkTable(request->tableID)) == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Incorrectly table id");
		printf("[logic]Error:Incorrectly table id\n");
		send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}
	if (checkFull(check) == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Table full");
		printf("[logic]Error: table fuul\n");
		send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}
	++countCurrentTables;
	int newSessison = getSession();
	newPlayer.session = newSessison;
	newPlayer.id = playersID++;
	newPlayer.money = 1000;
	strcpy(newPlayer.name, request->name);
	responce.status = STATUS_OK;
	responce.session = newSessison;
	responce.port = inofList[check].port;

	send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), (void *) &responce);
}

// int main() {

// 	printf("%d\n", getSession());
// 	return 0;
// }