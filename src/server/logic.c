#include "logic.h"

int flgReadFile = 0;
int countAllPlayers = 0;
int countCurrentTables = 0;
unsigned int playersID = 0;
unsigned int tableID = 0;
struct table_t lists[MAX_TABLES_COUNT]; /*Список столов*/

enum statusTable {EMPTY, SLEEP, FULL, PLAY};

struct infoOfTable {
	int countPlayer;
	int status;
} inofList[MAX_TABLES_COUNT];


int readFile() {
	FILE * fp;
	bzero(&data, sizeof(struct dataPlayers) * SIZE_DATA);
	if ((fp = fopen("data.txt", "rb")) == NULL) {
		system("touch data.txt");
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

void registration(void * buf) {

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
		// send(); /*Нет столов :(*/
		return;
	} else {
		void * buf = lists; /*Указатель на список столов*/
		// send( buf); /*отправляем список*/
	}
}
int getNewPort () {

	return 0;
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
	pid_t pid;
	struct newPlayer_t newPlayer;
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;

	int empt = findEmptyTable();
	if (empt == -1) {
		responce.status = STATUS_BAD;
		strcat(responce.error, "Невозможно создать стол!");
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}

	++countCurrentTables;
	newPlayer.session = getSession();
	newPlayer.id = playersID++;
	newPlayer.money = 1000;
	strcpy(newPlayer.name, request->name);

	lists[empt].id = tableID++;
	// lists[countCurrentTables].tables[] /*add name*/
	strcpy(lists[empt].tables[inofList[empt].countPlayer], request->name);

	pipe(pipedes);
	write(pipedes[0], (void *)&newPlayer, sizeof(struct newPlayer_t));
	pid = fork();
	if (pid == 0) { /*Дочерний*/
		close(pipedes[0]);
		startGameServer(pipedes[1], tableID);
		exit(0);
	} else {
		close(pipedes[1]);
		responce.port = getNewPort();
		responce.status = STATUS_OK;
		add_id_to_table(tableID, pipedes[0]);
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), (void *)&responce);
	}
}

void connectTable(void *buf) {
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;
}

// int main() {

// 	printf("%d\n", getSession());
// 	return 0;
// }