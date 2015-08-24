#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

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

#include "global.h"
#include "internalIPC.h"
#include "events.h"
#include "gameLogic.h"

#define DEFAULT_LISTEN_SERVER_IP "0.0.0.0"
#define DEFAULT_LISTEN_SERVER_PORT 1921


#define MSG_BUF_LEN 1024
#define MAX_ACTIVE_CONNECTIONS 30
#define MAX_TABLE_LEN 10

/* Создание слушающего сокета */
void init_listen_server_network(unsigned int listen_server_port, char *listen_server_ip);
/* Цикл приема сообщений */
void listen_server_loop(void);

/*
 * Создание сокета, добавление
 * файлового дескриптора слушающего сервера
 */
void init_game_server_network(int game_server_port, int listen_server_fd);
/* Цикл приема сообщений */
void game_server_loop(void);

/*
 * Функция передачи сообщений.
 * destination_type: для GAME_SERVER, CLIENT
 * указать destination_id. В остальных случаях
 * destination_id = 0.
 */
void send_message(int destination_type, int destination_id,
		int message_type, int message_len, void *message);

/*
 * Функция ИКЛЮЧИТЕЛЬНО для СЛУШАЮЩЕГО СЕРВЕРА.
 * Закрытие текущего соединения с клиентом
 */
void close_current_client_connection(void);

/*
 * Для идентификации соединений игровых
 * столов для слушающего сервера
 * и игроков на игровом сервере
 * используется таблица соответствий.
 * Добавление и удаление записи
 * в таблицу соответствия между id
 * и текущим обрабатываемым запросом.
 * При удалении записи закрывается
 * соединение. Если fd не известен
 * передавать 0.
 */
void add_id_to_table(int fd, int id);
void del_id_from_table(int fd, int id);

#endif /* SERVER_NETWORK_H */
