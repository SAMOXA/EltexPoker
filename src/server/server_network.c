#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "../global.h"
#include "internalIPC.h"
#include "events.h"
#include "server_network.h"

struct msg_hdr_t
{
	int type;
	int len;
};

static struct sockaddr_in listen_server_addr;

static int listen_socket;	/* Файловый дескриптор слушающего сокета */
static int active_connection_socket[MAX_ACTIVE_CONNECTION] = {0};	/* массив ФД активных соединений */
static int connections_count = 0;	/* количество активных соединений */
/* static int tables_socket[MAX_TABLES] = {0}; */

static int current_fd = 0;

/* обработка сигнала SIGPIPE */
static void sigpipe_handler(void){
	int i = 0;
	while((active_connection_socket[i] != 0) && (i < MAX_ACTIVE_CONNECTION))
		i++;
	printf("Client disconnected(SIGPIPE)\n");
	active_connection_socket[i] = 0;
	connections_count--;
}

/* Создание слушающего сокета */
void init_listen_server_network(void)
{
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (listen_socket < 0) {
		perror("[network] Creating listen socket, socket(): ");
		exit(1);
	}
	printf("[network] Listen socket created successfully\n");

	listen_server_addr.sin_family = AF_INET;
	listen_server_addr.sin_port = htons(LISTEN_SERVER_PORT);
	inet_aton(LISTEN_SERVER_IP, &listen_server_addr.sin_addr);

	if (bind(listen_socket, (struct sockaddr *) &listen_server_addr, sizeof(listen_server_addr))) {
		perror("[network] Binding port error, bind(): ");
		exit(1);
	}
	printf("[network] Port binded successfully\n");
}
/* Цикл приема сообщений */
void listen_server_loop(void)
{
	int i, j;
	int max_fd = 0;
	int new_client = 0;
	char buf[MSG_BUF_LEN];
	int bytes_recv = 0;
		
	fd_set fd_read_set;
	struct timeval select_interval;
	struct msg_hdr_t *buf_hdr;
	struct sockaddr_in new_client_addr;
	size_t new_client_addr_len = sizeof(struct sockaddr_in);

	signal(SIGPIPE, sigpipe_handler);

	listen(listen_socket, 5);

	while (1) {
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);
		max_fd = listen_socket;

		for(i = 0; i < MAX_ACTIVE_CONNECTION; i++){
			if(active_connection_socket[i] != 0)
				FD_SET(active_connection_socket[i], &fd_read_set);
			if(max_fd < active_connection_socket[i])
				max_fd = active_connection_socket[i];
		}
		select_interval.tv_sec = 0;
		select_interval.tv_usec = 1000;

		if (select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval) < 0)
			perror("[network] Listen server, select(): ");

		if (FD_ISSET(listen_socket, &fd_read_set)) {	/* обрабюотка нового подключения */
			new_client = accept(listen_socket, (struct sockaddr *) &new_client_addr, (socklen_t *)  &new_client_addr_len);
			if (new_client < 0) {
				perror("[network] Listen server accept error, accept(): ");
			}
			else {
				printf("[network] Connected new client: %s\n", inet_ntoa(new_client_addr.sin_addr));
				j = 0;
				while((active_connection_socket[j] != 0) && (j < MAX_ACTIVE_CONNECTION))
					j++;
				active_connection_socket[j] = new_client;
				connections_count++;
			}
			current_fd = new_client;
		}
		for (i = 0; i < connections_count; i++) {
			if (FD_ISSET(active_connection_socket[i], &fd_read_set)) {

				printf("FD_ISSET_cl\n");
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(active_connection_socket[i], buf, MSG_BUF_LEN);
				if (bytes_recv < 0) {
					perror("read");
					break;
				}
				current_fd = active_connection_socket[i];
				/* получить сообщение в соответствии с его длиной */

				/* вызвать events() */
				buf_hdr = (struct msg_hdr_t *) buf;

				events(CLIENT, 0, buf_hdr->type, (void *) (buf + 8));
			}
		}
	}
}

/*
 * Создание сокета, добавление
 * файлового дескриптора слушающего сервера
 */
void init_game_server_network(int listen_server_fd)
{
}
/* Цикл приема сообщений */
void game_server_loop()
{
}
/* Функция передачи сообщений */
void send_message(int destination_type, int destination_id,
                  int message_type, int message_len, void *message)
{
	struct msg_hdr_t *buf_hdr;
	char buf[MSG_BUF_LEN];

	switch (destination_type) {
	case SERVER:
		/* выбрать адрес по dest_id, добавить заголовок msg_hdr_t, отправить сообщение(pipe) */
		break;
	case CLIENT:
		/* выбрать адрес по dest_id, добавить заголовок msg_hdr_t, отправить сообщение(tcp) */
		break;
	case CURRENT:
		/* добавить заголовок msg_hdr_t, отправить сообщение через current_fd*/
		memset(buf, 0 , MSG_BUF_LEN);
		buf_hdr = (struct msg_hdr_t *) buf;
		buf_hdr->type = message_type;
		buf_hdr->len = message_len;
		memcpy(buf + sizeof(struct msg_hdr_t), message, message_len);

		if (write(current_fd, buf, MSG_BUF_LEN) < 0 ) {
			perror("write");
		}
		printf("Send\n");

		break;
	case ALL_CLIENTS:
		/* отпрвить сообщение всем ФД из players_fd */
		break;
	default:
		printf("[network] Send_message(): wrong destination argument\n");
		break;
	}
}

/* Закрытие текущего соединения с клиентом */
void close_current_connection(void)
{
}

/*
 * Добавление и удаление записи
 * соответствия между id и текущим
 * файловым дескриптором
 */
void add_id_to_table(int table_id, int id)
{
}
void del_id_from_table(int table_id, int id)
{
}
