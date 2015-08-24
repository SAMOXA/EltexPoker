#include "logic.h"

int countAllPlayers = 0; /*Кол-во всех игроков*/
int countCurrentTables = 0; /*Текущее кол-во столов*/
int playersID = 0; /*Последний id игрока*/
unsigned int tableID = 0;/*id таблицы*/
int currentPlayer = 0; /*текущий игрок*/

struct room_t room;
struct table_t lists[MAX_TABLES_COUNT]; /*Список столов*/
struct dataPlayers data[SIZE_DATA]; /*Информация по игрокам*/
struct infoOfTable inofList[MAX_TABLES_COUNT]; /*Информация по столам*/
struct listNameId IdName[MAX_TABLES_COUNT * MAX_PLAYERS_PER_TABLE]; /*Таблица хранит Name - ID*/
struct gamePort gPorts; /*Ифформация по портам*/
int freeId;

/*Чтение из файла*/
int readFile() {
	FILE * fp;
	bzero(&data, sizeof(struct dataPlayers) * SIZE_DATA);
	if ((fp = fopen("data.txt", "rb")) == NULL) {
		if (system("touch data.txt") < 0 ) {
			perror("system");
		}
		return -1;
	}
	while (fscanf(fp, "%d%s%s", &(data[countAllPlayers].id), (data[countAllPlayers].name), (data[countAllPlayers].pswd)) != EOF) {
		freeId = data[countAllPlayers].id + 1;
		countAllPlayers++;
	}
	fclose(fp);
	return 0;
}

/*Проверка имени*/
int checkName(char *name)
{
	int i;
	for (i = 0; i <= countAllPlayers; i++) {
		if (strcmp(name, data[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

/*Сохранение в файл имени и пароля*/
void saveFile()
{
	FILE *fp;
	int i;
	if ((fp = fopen("data.txt", "w+")) == NULL) {
		perror("[logic]Error - open file in saveFile():");
	}
	for (i = 0; i <= countAllPlayers; i++) {
		fprintf(fp, "%d %s %s\n", data[i].id, data[i].name, data[i].pswd);
	}
	fclose(fp);
}

void init(unsigned int port) {
	int i;
	bzero(&inofList, sizeof(struct infoOfTable) * MAX_TABLES_COUNT);
	bzero(&IdName, (sizeof(struct listNameId) * MAX_PLAYERS_PER_TABLE * MAX_TABLES_COUNT));
	gPorts.listPort[0] = port + 1;
	gPorts.statusListPort[0] = EMPTY;
	for (i = 1; i < MAX_TABLES_COUNT; i++) {
		gPorts.listPort[i] = ++gPorts.listPort[i - 1];
		gPorts.statusListPort[i] = EMPTY;
	}
}

/*Регистрация*/
void registration(void * buf)
{
	struct loginRequest_t *loginReq = (struct loginRequest_t *)buf;
	struct loginResponce_t *loginRes = malloc(sizeof(struct loginResponce_t));
	bzero(loginRes, sizeof(struct loginResponce_t));
	readFile();
	/*Проверяем имя*/
	if (checkName(loginReq->name) == -1) {
		strncpy(data[countAllPlayers].name, loginReq->name, MAX_NAME_LENGTH);
		strncpy(data[countAllPlayers].pswd, loginReq->pass, MAX_PASS_LENGTH);
		data[countAllPlayers].id = freeId;
		freeId++;
		saveFile();
		loginRes->status = STATUS_OK;
		send_message(CURRENT, 0, REGISTRATION, sizeof(struct loginResponce_t), (void *)loginRes);
		printf("[Logic]User %s registration\n", loginReq->name);
	} else {
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "Bad name");
		printf("[logic]User input bad name\n");
		send_message(CURRENT, 0, REGISTRATION, sizeof(struct loginResponce_t), (void *)loginRes);
	}
	free(loginRes);
}

/*Проверка пароля*/
int checkPasswd(int numInDataPlayers, char *pswd)
{
	if (strcmp(pswd, data[numInDataPlayers].pswd) == 0) {
		return 1;
	}
	return -1;
}

/*Вход*/
void login(void *buf)
{
	int checkNum;
	struct loginRequest_t *loginReq = (struct loginRequest_t *) buf;
	struct loginResponce_t *loginRes = malloc(sizeof(struct loginResponce_t));

	bzero(loginRes, sizeof(struct loginResponce_t));
	if (readFile() == -1) {
		/*Некоректное имя*/
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "User not found");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes);
		free(loginRes);
		return;
	}
	if ((checkNum = checkName(loginReq->name)) == -1) {
		/*Пользаватель с таким именем уже есть*/
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "User not found");
		printf("[Logic]User input incorrectly name\n");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes);
		free(loginRes);
		return;
	}
	if (checkPasswd(checkNum, loginReq->pass) == -1 ) {
		/*Некоректный пароль*/
		loginRes->status = STATUS_BAD;
		strcpy(loginRes->errorBuf, "Incorrectly password");
		printf("[Logic]User input incorrectly name\n");
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes); /*Некоректный пароль*/
		free(loginRes);
		return;
	} else {
		loginRes->status = STATUS_OK;
		playersID++;
		IdName[currentPlayer].id = playersID;
		strncpy(IdName[currentPlayer].name, loginReq->name, MAX_NAME_LENGTH);
		currentPlayer++;
		send_message(CURRENT, 0, LOG_IN, sizeof(struct loginResponce_t), (void *)loginRes);
		printf("[Logic]User %s log in\n", loginReq->name);
		free(loginRes);
		return;
	}

}

/*Получение списка столов*/
void tableList()
{
	if (!countCurrentTables) {
		room.status = ROOM_STATUS_EMPTY;

		/*В случае ошибки посылаем пустой буфер*/
		send_message(CURRENT, 0, LIST_TABLE, sizeof(struct room_t), (void*) &room);
		printf("[logic]Players get empty table list\n");

		return;

	} else {
		room.status = STATUS_OK;
		send_message(CURRENT, 0, LIST_TABLE, sizeof(struct room_t), (void*) &room);
		printf("[logic]Players get table list\n");
	}
}
/*Получение порта*/
int getNewPort()
{
	int i;
	for (i = 0; i < MAX_TABLES_COUNT; i++) {
		if (gPorts.statusListPort[i] == EMPTY) {
			gPorts.statusListPort[i] = PLAY;
			return gPorts.listPort[i];
		}
	}
	return -1;
}

/*Создание ключа сессии*/
int getSession()
{
	srand(time(NULL));
	return rand() / 10000;
}

/*Поиск пустого стола*/
int findEmptyTable()
{
	int i;
	for ( i = 0; i < MAX_TABLES_COUNT; i++) {
		if (inofList[i].status == EMPTY) {
			return i;
		}
	}
	return -1;
}

/*Получение ID по имени из таблицы*/
int getIDtoName(char *name)
{
	int i;
	for (i = 0; i <= currentPlayer; i++) {
		if (!strcmp(name, IdName[i].name)) {
			return IdName[i].id;
		}
	}
	return -1;
}

/*Создание стола*/
void createTable(void *buf)
{
	int pipedes[2];
	int newSessison;
	pid_t pid;
	struct newPlayer_t newPlayer;
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;
	int empt = findEmptyTable();
	/*Поиск пустого стола*/
	if (empt == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "No empty tables");
		printf("[logic]We not have empty tables\n");
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}

	int id = getIDtoName(request->name);
	/*Проверяем если ли такой пользователь в системе*/
	if (id == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "You not login");
		printf("[logic]No login user try create table\n");
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}
	++countCurrentTables;
	newSessison = getSession();
	newPlayer.session = newSessison;
	newPlayer.id = id;
	/*Money*/
	newPlayer.money = 1000;
	strncpy(newPlayer.name, request->name, MAX_NAME_LENGTH);
	tableID++;
	room.tables[empt].id = tableID;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipedes) < 0 ) {
		perror("pipe");
	}
	responce.port = getNewPort();
	pid = fork();
	if (pid == 0) { /*Дочерний*/
		close(pipedes[1]);
		startGameServer(pipedes[0], tableID, responce.port);

		exit(0);
	} else {
		close(pipedes[0]);
		/*Получаем новый порт*/
		if (responce.port == -1) {
			responce.status = STATUS_BAD;
			strcpy(responce.error, "Not have port");
			printf("[logic]We have not new port\n");
			send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
			return;
		}
		inofList[empt].port = responce.port;
		responce.status = STATUS_OK;
		responce.session = newSessison;
		responce.id = id;
		printf("Logic id after fork %d\n", id);
		add_id_to_table(pipedes[1], tableID);

		send_message(GAME_SERVER, tableID, INTERNAL_NEW_PLAYER, sizeof(struct newPlayer_t), (void *) &newPlayer);
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), (void *)&responce);
		/*Close connect*/
		close_current_client_connection();
		printf("[logic]Success create table and disconnect client\n");
	}
}

/*Проверка наличия id в таблице*/
int checkTable(int id)
{
	int i;
	for (i = 0; i < MAX_TABLES_COUNT; i++) {
		if (room.tables[i].id == id) {
			return i;
		}
	}
	return -1;
}

/*Проверка на полнотау сервера*/
int checkFull(int num)
{
	if ((inofList[num].countPlayer == MAX_PLAYERS_PER_TABLE) || (inofList[num].status == PLAY)) {
		return -1;
	}
	return 1;
}

/*Соединение к столу*/
void connectTable(void *buf)
{
	struct newPlayer_t newPlayer;
	struct selectRequest_t *request = (struct selectRequest_t *) buf;
	struct selectResponce_t responce;
	int check, id;
	/*Проверка id стола*/
	if ((check = checkTable(request->tableID)) == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Incorrectly table id");
		printf("[logic]User send incorrectly table id\n");
		send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}
	/*Проверка колличества игроков на столе*/
	if (checkFull(check) == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Table full");
		printf("[logic]User try connect to full table\n");
		send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}

	id = getIDtoName(request->name);
	/*Нет id*/
	if (id == -1) {
		responce.status = STATUS_BAD;
		strcpy(responce.error, "Incorrectly id");
		printf("[logic]User with incorrectly id\n");
		send_message(CURRENT, 0, CREATE_TABLE, sizeof(struct selectResponce_t), &responce);
		return;
	}
	++countCurrentTables;
	int newSessison = getSession();
	newPlayer.session = newSessison;
	newPlayer.money = 1000;
	newPlayer.id = id;

	strncpy(newPlayer.name, request->name, MAX_NAME_LENGTH);
	responce.status = STATUS_OK;
	responce.session = newSessison;
	responce.port = inofList[check].port;
	responce.id = id;

	send_message(GAME_SERVER, tableID, INTERNAL_NEW_PLAYER, sizeof(struct newPlayer_t), (void *) &newPlayer);
	send_message(CURRENT, 0, CONNECT_TO_TABLE, sizeof(struct selectResponce_t), (void *) &responce);
	close_current_client_connection();
	printf("[Logic]User connect o table\n");
}
/*Получение имени по id из таблицы*/
int getNameToID(int id)
{
	int i;
	for (i = 0; i < currentPlayer; i++) {
		if (IdName[i].id == id) {
			return i;
		}
	}
	return -1;
}

/*Подтверждение коннекта*/
void confirmedConnect(void *buf, int serverID)
{
	int *clID = (int *)buf;
	int IDforName = getNameToID(*clID);
	int num = checkTable(*clID);
	inofList[num].countPlayer++;
	(inofList[num].countPlayer == 4) ? (inofList[num].status = FULL) : (inofList[num].status = SLEEP);
	strncpy(room.tables[num].players[inofList[num].countPlayer], IdName[IDforName].name, MAX_NAME_LENGTH);
	printf("[logic] Player confirmed %d %s\n", *clID, IdName[IDforName].name);
}

/*Удаление игрока*/
void removePlayerFromTable(void *buf)
{
	int id = *((int *)buf);
	int i, index;
	int max_players = MAX_TABLES_COUNT * MAX_PLAYERS_PER_TABLE;
	char remove_name[256];

	/* 1) Searching player's name by id */
	for (index = 0; index < max_players; index++) {
		if (IdName[index].id == id) {
			/* If name found */
			strncpy(remove_name, IdName[index].name, MAX_NAME_LENGTH);
			break;
		}
	}

	/* 2) Searching requested player by name */
	for (index = 0; index < MAX_TABLES_COUNT; index++) {
		for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) {
			if ((strcmp(room.tables[index].players[i], remove_name)) == 0) {
				/* If name found */
				bzero(room.tables[index].players[i], MAX_NAME_LENGTH);
				/* 3) Decrement structure "information of table" */
				inofList[index].countPlayer -= 1;
				if (inofList[index].countPlayer == 0) {
					removeTable(room.tables[index].id);
				}
				return;
			}
		}
	}
}
/*Удаление стола*/
void removeTable(int id)
{
	int i, index;

	/* Searching requested table by id */
	for (index = 0; index < MAX_TABLES_COUNT; index++) {
		if (id == room.tables[index].id) {
			break;
		}
	}
	/* If table found */

	/* 1) Clean up structure "table" */
	for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) {
		bzero(room.tables[index].players[i], MAX_NAME_LENGTH);
	}

	/* 2) Clean up structure "information of table" */
	inofList[index].status 	= EMPTY;
	inofList[index].countPlayer = 0;
	inofList[index].port = 0;
	for (i = 0; i < MAX_TABLES_COUNT; i++) {
		if (inofList[i].port == gPorts.listPort[i]) {
			gPorts.statusListPort[i] = EMPTY;
		}
	}
	del_id_from_table(0, id);
}

