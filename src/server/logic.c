#include "logic.h"

int flgReadFile = 0;
int countAllPlayers = 0;
int countCurrentTables = 0;
struct table_t lists[MAX_TABLES_COUNT]; /*Список столов*/

static int readFile() {
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

static int checkName(char *name) {
	int i;
	for (i = 0; i <= countAllPlayers; i++) {
		if (strcmp(name, data[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

static void saveFile() {
	FILE *fp;
	int i;
	if ((fp = fopen("data.txt", "w+")) == NULL) {

	}

	for (i = 0; i <= countAllPlayers; i++) {
		fprintf(fp, "%s %s\n", data[i].name, data[i].pswd);

	}
	fclose(fp);
}

static void registration(void * buf) {

	// struct loginRequest_t * login = (struct loginRequest_t *)buf;

	readFile();
	if (checkName((char *)buf/*!!!!*/) == -1) {
		strcpy(data[countAllPlayers].name, (char *)buf);
		strcpy(data[countAllPlayers].pswd, "123");
		saveFile();
		// send();/*Зарегестрирован*/
	} else {
		// send(); /*Error*/
	}
}

static int checkPasswd(int numCheck, char *pswd) {
	if (strcmp(pswd, data[numCheck].pswd) == 0) {
		return 1;
	}
	return -1;
}

static void login(void *buf) {
	int checkNum;
	if (readFile() == -1) {

		// send(); /*Некоректное имя*/
	}
	if ((checkNum = checkName((char *)buf)) == -1) {
		printf("LOG_IN bad1\n");
		// send()/*Пользаватель с таким именем уже есть*/
		return;
	}
	char pass[] = {"12"};
	if (checkPasswd(checkNum, pass) == -1 ) {
		printf("LOG_IN bad2\n");
		// send()/*Некоректный пароль*/
		return;
	} else {
		printf("LOG_IN OK\n");
		// send(); /*Залогиннился и отправка столов*/
	}
}

static void tableList() {
	if (!countCurrentTables) {
		// send(); /*Нет столов :(*/
		return;
	} else {
		void * buf = lists; /*Указатель на список столов*/
		// send( buf); /*отправляем список*/
	}

}
// int main() {

// 	char n[] = {"lo"};
// 	// registration((void *)n);
// 	login((void *)n);
// 	return 0;
// }