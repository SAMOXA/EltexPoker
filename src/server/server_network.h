#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#define LISTEN_SERVER_IP "192.168.1.24"
#define LISTEN_SERVER_PORT 1921

#define MSG_BUF_LEN 1024
#define MAX_ACTIVE_CONNECTIONS 30
#define MAX_TABLE_LEN 10


/* Получение индекса записи в таблицe по ИД */
static int get_index_by_id(int id);
/* Получение индекса записи в таблицe по ФД */
static int get_index_by_fd(int fd);

/* Создание слушающего сокета */
void init_listen_server_network(void);
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
 * Добавление и удаление записи
 * в таблицу соответствия между id
 * и текущим файловым дескриптором.
 * При удалении записи закрывается
 * соединение.
 */
void add_id_to_table(int table_id, int id);
void del_id_from_table(int table_id, int id);

#endif /* SERVER_NETWORK_H */
