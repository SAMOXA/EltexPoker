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

#include "../global.h"
#include "server_network.h"


static struct sockaddr_in listen_server_addr;

static int listen_socket;
static int active_connection_socket[MAX_ACTIVE_CONNECTION] = {0};
static int connections_count = 0;

/* Создание слушающего сокета */
void init_listen_server_network(void)
{
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if(listen_socket < 0){
		perror("creating listen socket, socket(): ");
		return -1;
	}
	printf("listen socket created successfully\n");

	listen_server_addr.sin_family = AF_INET;
	listen_server_addr.sin_port = htons(LISTEN_SERVER_PORT);
	inet_aton(LISTEN_SERVER_IP, &listen_server_addr.sin_addr);

	if(bind(listen_socket, (struct sockaddr *) &listen_server_addr, sizeof(listen_server_addr))){
		perror("binding port error, bind(): ");
		return -1;
	}
	printf("port binded successfully\n");
}
/* Цикл приема сообщений */
void listen_server_loop(void)
{
	int i;
	int max_fd = 0;
	fd_set fd_read_set;
	struct timeval select_interval;

	listen(listen_socket, 5);

	while(1){
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);
		max_fd = listen_socket;

		for(i = 0; i < connections_count; i++){
			FD_SET(active_connection_socket[i], &fd_read_set);
			if(max_fd < active_connection_socket[i])
				max_fd = active_connection_socket[i];
		}
		select_interval.tv_sec = 0;
		select_interval.tv_usec = 100;

		return_val = select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval);
		if(return_val < 0)
			perror("listen server select, select(): ");

		if(FD_ISSET(listen_socket, &fd_read_set)){
			new_client_fd = accept(listen_socket, (struct sockaddr *) &new_client_addr,(socklen_t *)  &new_client_addr_len);
			if(new_client_fd < 0){
				perror("listen server accept error, accept(): ");
			}
			else{
			printf("connected %d\n", new_client_addr.sin_port);
			fflush(stdout);
			active_connection_socket[connections_count] = new_client_fd;
			connections_count++;
		}
		for(i = 0; i < connections_count; i++){
			if(FD_ISSET(active_connection_socket[i], &fd_read_set)){
				bytes_recv = recv(active_connection_socket[i], buf, MSG_BUF_LEN, 0);
				/* получить сообщение в соответствии с его длиной */
				/* вызвать events() */
			}
		}
	}
}

/* 
 * Создание сокета, добавление
 * файлового дескриптора слушающего сервера
 */
void init_game_server_network(int listen_server_fd);
/* Цикл приема сообщений */
void game_server_loop();

/* Функция передачи сообщений */
void send_message(int destination_type, int destination_id,
		int message_type, int message_len, void *message);
		
/* Закрытие текущего соединения с клиентом */
void close_current_connection(void);

/*
 * Добавление и удаление записи
 * соответствия между id и текущим 
 * файловым дескриптором
 */
void add_id_to_table(int table_id, int id);
void del_id_from_table(int table_id, int id);
