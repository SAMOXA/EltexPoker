#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#define LISTEN_SERVER_IP "192.168.1.1"
#define LISTEN_SERVER_PORT 1921

#define MSG_BUF_LEN 1024
#define MAX_ACTIVE_CONNECTION 30

/* Создание слушающего сокета */
void init_listen_server_network(void);
/* Цикл приема сообщений */
void listen_server_loop(void);

/* 
 * Создание сокета, добавление
 * файлового дескриптора слушающего сервера
 */
void init_game_server_network(int listen_server_fd);
/* Цикл приема сообщений */
void game_server_loop(void);

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

#endif /* SERVER_NETWORK_H */
