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
#include "events.h"
#include "server_network.h"

struct msg_hdr_t
{
	int type;
	int len;
};

static struct sockaddr_in listen_server_addr;
static struct sockaddr_in game_server_addr;

static int listen_socket;	/* Файловый дескриптор слушающего сокета */
static int pipe_fd;			/* Для общения между игровым и слушающим серверами */
static int active_connection_socket[MAX_ACTIVE_CONNECTION] = {0};	/* массив ФД активных соединений */
static int connections_count = 0;	/* количество активных соединений */
static int fd_table[MAX_TABLES][2] = {{0}};	/* Для хранения ИД и ФД игровых столов */

static int current_fd = 0;

static fd_set fd_read_set;


/* Получение индекса записи в таблицe по ИД */
static int get_index_by_id(int id){
	int i = 0;
	
	/* table_socket[x][0] - id*/
	while((fd_table[i][0] != id) && (i < MAX_TABLES))
		i++;
	if(i == MAX_TABLES)
		return -1;
	return i;	/* Возврат индекс переданного ИД */
}

/* Создание слушающего сокета */
void init_listen_server_network(void)
{
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if(listen_socket < 0){
		perror("[network] Creating listen socket, socket(): ");
		exit(1);
	}
	printf("[network] Listen socket created successfully\n");

	memset((void *) &listen_server_addr, 0, sizeof(struct sockaddr_in));
	listen_server_addr.sin_family = AF_INET;
	listen_server_addr.sin_port = htons(LISTEN_SERVER_PORT);
	inet_aton(LISTEN_SERVER_IP, &listen_server_addr.sin_addr);

	if(bind(listen_socket, (struct sockaddr *) &listen_server_addr, sizeof(listen_server_addr))){
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
		
	struct timeval select_interval;
	struct msg_hdr_t *buf_hdr;
	struct sockaddr_in new_client_addr;
	size_t new_client_addr_len = sizeof(struct sockaddr_in);

	listen(listen_socket, 5);

	while(1){
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);
		max_fd = listen_socket;
		
		if(connections_count > 0){
			for(i = 0; i < MAX_ACTIVE_CONNECTION; i++){
				if(active_connection_socket[i] != 0)
					FD_SET(active_connection_socket[i], &fd_read_set);
				if(max_fd < active_connection_socket[i])
					max_fd = active_connection_socket[i];
			}
		}
		select_interval.tv_sec = 0;
		select_interval.tv_usec = 100000UL;

		if(select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval) < 0)
			perror("[network] Listen server, select(): ");

		if(FD_ISSET(listen_socket, &fd_read_set)){	/* обрабюотка нового подключения */
			new_client = accept(listen_socket, (struct sockaddr *) &new_client_addr,(socklen_t *)  &new_client_addr_len);
			if(new_client < 0){
				perror("[network] Listen server accept error, accept()");
			}
			else{
				printf("[network] New client connected: %s\n", inet_ntoa(new_client_addr.sin_addr));
				j = 0;
				while((active_connection_socket[j] != 0) && (j < MAX_ACTIVE_CONNECTION))
					j++;
				active_connection_socket[j] = new_client;
				connections_count++;
				current_fd = new_client;
			}
		}
		for(i = 0; i < MAX_ACTIVE_CONNECTION; i++){
			if(FD_ISSET(active_connection_socket[i], &fd_read_set)){
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(active_connection_socket[i], buf, MSG_BUF_LEN);
				
				if(bytes_recv < 0){
                	perror("[network] read()");
                	current_fd = active_connection_socket[i];
                	close_current_connection();
                }
                else if(bytes_recv == 0){
                	printf("[network] read() returned 0, closing connection\n");
                	current_fd = active_connection_socket[i];
                	close_current_connection();
                }
				else{
					current_fd = active_connection_socket[i];
					/* получить сообщение в соответствии с его длиной */
				
					/* вызвать events(), передать параметры и сообщение */
					buf_hdr = (struct msg_hdr_t *) buf;
					events(0, 0, buf_hdr->type, (void *) (buf + 8));
				}
			}
		}
	}
}

/* 
 * Создание сокета, добавление
 * файлового дескриптора слушающего сервера
 */
void init_game_server_network(int game_server_port, int listen_server_fd)
{
	int i;
	
	for(i = 0; i < MAX_ACTIVE_CONNECTION; i++)
		active_connection_socket[i] = 0;
	
	pipe_fd = listen_server_fd;
	connections_count = 0;
	
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if(listen_socket < 0){
		perror("[game_server_network] Creating listen socket, socket(): ");
		exit(1);
	}
	printf("[game_server_network] Listen socket created successfully\n");

	memset((void *) &game_server_addr, 0, sizeof(struct sockaddr_in));
	game_server_addr.sin_family = AF_INET;
	game_server_addr.sin_port = htons(game_server_port);
	inet_aton(LISTEN_SERVER_IP, &game_server_addr.sin_addr);

	if(bind(listen_socket, (struct sockaddr *) &game_server_addr, sizeof(game_server_addr))){
		perror("[game_server_network] Binding port error, bind(): ");
		exit(1);
	}
	printf("[game_server_network] Port binded successfully\n");
}
/* Цикл приема сообщений */
void game_server_loop()
{
	int i, j;
	int max_fd = 0;
	int new_client = 0;
	char buf[MSG_BUF_LEN];
	int bytes_recv = 0;
		
	struct timeval select_interval;
	struct msg_hdr_t *buf_hdr;
	struct sockaddr_in new_client_addr;
	size_t new_client_addr_len = sizeof(struct sockaddr_in);

	listen(listen_socket, 5);

	while(1){
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);
		FD_SET (pipe_fd, &fd_read_set);
		max_fd = listen_socket > pipe_fd ? listen_socket : pipe_fd;
		
		if(connections_count > 0){
			for(i = 0; i < MAX_ACTIVE_CONNECTION; i++){
				if(active_connection_socket[i] != 0)
					FD_SET(active_connection_socket[i], &fd_read_set);
				if(max_fd < active_connection_socket[i])
					max_fd = active_connection_socket[i];
			}
		}
		select_interval.tv_sec = 0;
		select_interval.tv_usec = 100000UL;

		if(select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval) < 0)
			perror("[game_server_network] Game server, select(): ");

		if(FD_ISSET(listen_socket, &fd_read_set)){	/* обрабюотка нового подключения */
			new_client = accept(listen_socket, (struct sockaddr *) &new_client_addr,(socklen_t *)  &new_client_addr_len);
			if(new_client < 0){
				perror("[game_server_network] Game server accept error, accept()");
			}
			else{
				printf("[game_server_network] New client connected: %s\n", inet_ntoa(new_client_addr.sin_addr));
				j = 0;
				while((active_connection_socket[j] != 0) && (j < MAX_ACTIVE_CONNECTION))
					j++;
				active_connection_socket[j] = new_client;
				connections_count++;
				current_fd = new_client;
			}
		}
		for(i = 0; i < MAX_ACTIVE_CONNECTION; i++){
			if(FD_ISSET(active_connection_socket[i], &fd_read_set)){
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(active_connection_socket[i], buf, MSG_BUF_LEN);
				
				if(bytes_recv < 0){
                	perror("[game_server_network] read()");
                	current_fd = active_connection_socket[i];
                	close_current_connection();
                }
                else if(bytes_recv == 0){
                	printf("[game_server_network] read() returned 0, closing connection\n");
                	current_fd = active_connection_socket[i];
                	close_current_connection();
                }
				else{
					current_fd = active_connection_socket[i];
					/* получить сообщение в соответствии с его длиной */
				
					/* вызвать events(), передать параметры и сообщение */
					buf_hdr = (struct msg_hdr_t *) buf;
					events(0, 0, buf_hdr->type, (void *) (buf + 8));
				}
			}
		}
	}
}
/* Функция передачи сообщений */
void send_message(int destination_type, int destination_id,
		int message_type, int message_len, void *message)
{
	struct msg_hdr_t *buf_hdr;
	char buf[MSG_BUF_LEN];
	int return_val = 0;

	switch(destination_type){
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
			
			return_val = write(current_fd, buf, MSG_BUF_LEN);
			if(return_val < 0){
                perror("[network] write()");
				close_current_connection();
            }
            if(return_val == 0){
               	printf("[network] write() returned 0, closing connection\n");
               	close_current_connection();
            }            
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
	int i = 0;
	while(active_connection_socket[i] != current_fd)
    	i++;
    close(active_connection_socket[i]);
    active_connection_socket[i] = 0;
	connections_count--;
}

/*
 * Добавление и удаление записи
 * соответствия между id и текущим 
 * файловым дескриптором
 */
void add_id_to_table(int table_id, int id)
{
	int i = get_index_by_id(0);	/* поиск свободной ячейки*/
	
	if(i < 0){	/* ошибка */
		printf("[network] add_id_to_table(): table with id %d overflowed\n", table_id);
	}
	else{
		fd_table[i][0] = id;
		fd_table[i][1] = current_fd;
	}
}
void del_id_from_table(int table_id, int id)
{
	int i = get_index_by_id(0);	/* поиск свободной ячейки*/
	
	if(i < 0){	/* ошибка */
		printf("[network] del_id_to_table(): cant find entry with id = %d\n", id);
	}
	else{
		fd_table[i][0] = 0;
		fd_table[i][1] = 0;
	}
}
