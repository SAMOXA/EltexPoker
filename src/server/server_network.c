#include "server_network.h"

static struct sockaddr_in listen_server_addr;
static struct sockaddr_in game_server_addr;

static int listen_socket;	/* Файловый дескриптор слушающего сокета */
static int pipe_fd = 0;		/* Для общения между игровым и слушающим серверами */
static char defined_ip[24] = {0};
/*
 * Массив ФД активных соединений.
 * Хранит незарегистрированных клиентов
 * на стороне слушающего сервера и
 * клиентов еще не получивщих разрешение
 * на поключение к игровому столу
 * на стороне игрового сервера.
 */
static int active_connection_socket[MAX_ACTIVE_CONNECTIONS] = {0};
static int connections_count = 0;	/* количество активных соединений клиентов */
static int tables_count = 0;	/* количество активных игровых столов */
static int fd_table[MAX_TABLE_LEN][2] = {{0}};	/* Для хранения ИД и ФД игровых столов */

/*
 * При передаче управление в другой
 * модуль в current_fd сохраняется ФД
 * соединения, запрос чей запрос нужно
 * обработать. Используется для отправки
 * ответа, закрытия соединения
 */
static int current_fd = 0;

static fd_set fd_read_set;


/* Получение индекса записи в таблицe по ИД */
static int get_index_by_id(int id){
	int i = 0;

	/* table_socket[x][0] -- id*/
	while((fd_table[i][0] != id) && (i < MAX_TABLE_LEN))
		i++;
	if(i == MAX_TABLE_LEN)
		return -1;
	return i;	/* Возврат индекс переданного ИД */
}

/* Получение индекса записи в таблицe по ФД */
static int get_index_by_fd(int fd){
	int i = 0;

	/* table_socket[x][1] -- fd*/
	while((fd_table[i][1] != fd) && (i < MAX_TABLE_LEN))
		i++;
	if(i == MAX_TABLE_LEN)
		return -1;
	return i;	/* Возврат индекс переданного ИД */
}

/* Создание слушающего сокета */
void init_listen_server_network(unsigned int listen_server_port, char *listen_server_ip)
{
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (listen_socket < 0) {
		perror("[network] Creating listen socket, socket(): ");
		exit(1);
	}
	printf("[listen_server_network] Listen socket created successfully\n");

	memset((void *) &listen_server_addr, 0, sizeof(struct sockaddr_in));
	listen_server_addr.sin_family = AF_INET;
	
	if((listen_server_port < 1024) || (listen_server_port > 65000)){
		printf("[listen_server_network] Invalid PORT: %d\n", listen_server_port);
		listen_server_addr.sin_port = htons(DEFAULT_LISTEN_SERVER_PORT);
	}
	else
		listen_server_addr.sin_port = htons(listen_server_port);

	if(listen_server_ip != 0)
		if(inet_aton(listen_server_ip, &listen_server_addr.sin_addr) != 0){
			memset(defined_ip, 0, 24);
			memcpy(defined_ip, listen_server_ip, strlen(listen_server_ip));
		}
		else{
			printf("[listen_server_network] Invalid IP adress: %s\n", listen_server_ip);
			inet_aton(DEFAULT_LISTEN_SERVER_IP, &listen_server_addr.sin_addr);
		}
	else{
		inet_aton(DEFAULT_LISTEN_SERVER_IP, &listen_server_addr.sin_addr);
	}

	if (bind(listen_socket, (struct sockaddr *) &listen_server_addr, sizeof(listen_server_addr))) {
		perror("[network] Binding port error, bind(): ");
		exit(1);
	}
	printf("[listen_server_network] Port binded successfully. PORT: %d, IP: %s\n", htons(listen_server_addr.sin_port), inet_ntoa(listen_server_addr.sin_addr));
}

/* Цикл приема сообщений слушающего сервера */
void listen_server_loop(void)
{
	int i, j;
	int max_fd = 0;
	int new_client = 0;
	char buf[MSG_BUF_LEN];
	int bytes_recv = 0;
	int return_val;

	struct timeval select_interval;
	struct network_msg_hdr_t *net_header;
	struct sockaddr_in new_client_addr;
	size_t new_client_addr_len = sizeof(struct sockaddr_in);

	if(listen(listen_socket, 5) < 0){
		perror("[listen_server_network] listen()");
		exit(1);
	}

	while (1) {
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);
		max_fd = listen_socket;

		/* Добавление ФД клиентов */
		if(connections_count > 0){
			for(i = 0; i < MAX_ACTIVE_CONNECTIONS; i++){
				if(active_connection_socket[i] != 0)
					FD_SET(active_connection_socket[i], &fd_read_set);
				if(max_fd < active_connection_socket[i])
					max_fd = active_connection_socket[i];
			}
		}

		/* Добавление ФД игровых столов */
		if(tables_count > 0){
			for(i = 0; i < MAX_TABLE_LEN; i++){
				if(fd_table[i][1] != 0)
					FD_SET(fd_table[i][1], &fd_read_set);
				if(max_fd < fd_table[i][1])
					max_fd = fd_table[i][1];
			}
		}

		select_interval.tv_sec = 0;
		select_interval.tv_usec = 100000UL;

		if(select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval) < 0)
			perror("[listen_server_network] Listen server, select(): ");

		if(FD_ISSET(listen_socket, &fd_read_set)){	/* обработка нового подключения */
			new_client = accept(listen_socket, (struct sockaddr *) &new_client_addr,(socklen_t *)  &new_client_addr_len);
			if(new_client < 0){
				perror("[listen_server_network] Listen server accept error, accept()");
			}
			else{
				printf("[listen_server_network] New client connected: %s\n", inet_ntoa(new_client_addr.sin_addr));
				j = 0;
				while((active_connection_socket[j] != 0) && (j < MAX_ACTIVE_CONNECTIONS))
					j++;
				active_connection_socket[j] = new_client;
				connections_count++;
				current_fd = new_client;
			}
			current_fd = new_client;
		}
		for(i = 0; i < MAX_ACTIVE_CONNECTIONS; i++){	/* Сообщения от клиентов */
			if(FD_ISSET(active_connection_socket[i], &fd_read_set)){
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(active_connection_socket[i], buf, MSG_BUF_LEN);

				if(bytes_recv < 0){
					perror("[listen_server_network] read()");
					current_fd = active_connection_socket[i];
					close_current_client_connection();
				}
				else if(bytes_recv == 0){
					printf("[listen_server_network] read() returned 0, closing connection\n");
					current_fd = active_connection_socket[i];
					close_current_client_connection();
				}
				else{
					current_fd = active_connection_socket[i];
					/* получить сообщение в соответствии с его длиной */
					net_header = (struct network_msg_hdr_t *) buf;
					events(CURRENT, 0, net_header->payload_type, (void *) (buf + 8));
				}
			}
		}

		for(i = 0; i < MAX_TABLE_LEN; i++){	/* Сообщения от игровых столов */
			if(FD_ISSET(fd_table[i][1], &fd_read_set)){
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(fd_table[i][1], buf, MSG_BUF_LEN);

				if(bytes_recv < 0){
					perror("[listen_server_network] read()");
					current_fd = fd_table[i][1];
					del_id_from_table(0, fd_table[i][0]);
				}
				else if(bytes_recv == 0){
					printf("[listen_server_network] read() returned 0, closing connection\n");
					current_fd = fd_table[i][1];
					del_id_from_table(0, fd_table[i][0]);
				}
				else{
					current_fd = fd_table[i][1];
					/* получить сообщение в соответствии с его длиной */
					/* вызвать events(), передать параметры и сообщение */
					net_header = (struct network_msg_hdr_t *) buf;
					/* Получение индекса записи с ИД игрового сервера */
					return_val = get_index_by_fd(fd_table[i][1]);

					if(return_val < 0){
						printf("[listen_server_network] get_index_by_fd() returned -1, cant find entry with id = %d\n", fd_table[i][0]);
						events(GAME_SERVER, 0, net_header->payload_type, (void *) (buf + 8));
					}
					else
						events(GAME_SERVER, return_val, net_header->payload_type, (void *) (buf + 8));
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

	for(i = 0; i < MAX_ACTIVE_CONNECTIONS; i++)
		active_connection_socket[i] = 0;

	for(i = 0; i < MAX_TABLE_LEN; i++){
		fd_table[i][0] = 0;
		fd_table[i][1] = 0;
	}

	pipe_fd = listen_server_fd;
	current_fd = listen_server_fd;
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
	if(strlen(defined_ip)){	/* Если слушающий сервер установил новый IP */
		inet_aton(defined_ip, &game_server_addr.sin_addr);
		printf("[game_server_network] Game server IP: %s\n", defined_ip);
	}
	else{
		inet_aton(DEFAULT_LISTEN_SERVER_IP, &game_server_addr.sin_addr);
		printf("[game_server_network] Game server IP: %s\n", DEFAULT_LISTEN_SERVER_IP);
	}

	if(bind(listen_socket, (struct sockaddr *) &game_server_addr, sizeof(game_server_addr))){
		perror("[game_server_network] Binding port error, bind(): ");
		exit(1);
	}
	printf("[game_server_network] Port binded successfully. PORT: %d\n", game_server_port);
}

/* Цикл приема сообщений игрового сервера */
void game_server_loop()
{
	int i, j;
	int max_fd = 0;
	int new_client = 0;
	char buf[MSG_BUF_LEN];
	int bytes_recv = 0;
	int return_val;

	struct timeval select_interval;
	struct network_msg_hdr_t *net_header;
	struct sockaddr_in new_client_addr;
	size_t new_client_addr_len = sizeof(struct sockaddr_in);

	if(listen(listen_socket, 5) < 0){
		perror("[listen_server_network] listen()");
		exit(1);
	}

	while(1){
		FD_ZERO (&fd_read_set);
		FD_SET (listen_socket, &fd_read_set);

		if(pipe_fd > 0)
			FD_SET (pipe_fd, &fd_read_set);	/* Для взаимодействия со слушающим сервером */

		max_fd = listen_socket > pipe_fd ? listen_socket : pipe_fd;

		/* Добавление ФД игроков */
		if(tables_count > 0){
			for(i = 0; i < MAX_TABLE_LEN; i++){
				if(fd_table[i][1] != 0)
					FD_SET(fd_table[i][1], &fd_read_set);
				if(max_fd < fd_table[i][1])
					max_fd = fd_table[i][1];
			}
		}

		select_interval.tv_sec = 0;
		select_interval.tv_usec = 100000UL;

		if(select(max_fd + 1, &fd_read_set, NULL, NULL, &select_interval) < 0)
			perror("[game_server_network] Game server, select(): ");

		if(FD_ISSET(listen_socket, &fd_read_set)){	/* обработка нового подключения */
			new_client = accept(listen_socket, (struct sockaddr *) &new_client_addr,(socklen_t *)  &new_client_addr_len);
			if(new_client < 0){
				perror("[game_server_network] Game server accept error, accept()");
			}
			else{
				printf("[game_server_network] New client connected: %s\n", inet_ntoa(new_client_addr.sin_addr));
				j = 0;
				while((active_connection_socket[j] != 0) && (j < MAX_ACTIVE_CONNECTIONS))
					j++;
				active_connection_socket[j] = new_client;
				connections_count++;
				current_fd = new_client;
			}
		}

		if(FD_ISSET(pipe_fd, &fd_read_set) && pipe_fd > 0){	/* обработка сообщения от слушающего сервера */
			memset(buf, 0, MSG_BUF_LEN);
			bytes_recv = 0;
			bytes_recv = read(pipe_fd, buf, MSG_BUF_LEN);

			if(bytes_recv < 0){
               	perror("[game_server_network] reading form listen_server: read()");
               	current_fd = pipe_fd;
               	printf("[game_server_network] closing listen_server connection\n");
               	if(close(pipe_fd) < 0){
               		printf("[game_server_network] closing error:\n");
               		perror("close()");
               	}
               	pipe_fd = 0;
            }
            else if(bytes_recv == 0){
              	printf("[game_server_network] reading form listen_server: read() returned 0, closing connection\n");
               	current_fd = pipe_fd;
               	if(close(pipe_fd) < 0){
               		printf("[game_server_network] closing error:\n");
               		perror("close()");
               	}
               	pipe_fd = 0;
            }
			else{
				current_fd = pipe_fd;
				/* получить сообщение в соответствии с его длиной */

				/* вызвать events(), передать параметры и сообщение */
				net_header = (struct network_msg_hdr_t *) buf;

				gameEvents(LOBBY_SERVER, 0, net_header->payload_type, (void *) (buf + 8));
			}
		}

		for(i = 0; i < MAX_TABLE_LEN; i++){	/* Сообщения от клиентов */
			if(FD_ISSET(fd_table[i][1], &fd_read_set)){
				memset(buf, 0, MSG_BUF_LEN);
				bytes_recv = 0;
				bytes_recv = read(fd_table[i][1], buf, MSG_BUF_LEN);

				if(bytes_recv < 0){
                	perror("[game_server_network] read()");
                	current_fd = fd_table[i][1];
                	del_id_from_table(0, fd_table[i][0]);
                }
                else if(bytes_recv == 0){
                	printf("[game_server_network] read() returned 0, closing connection\n");
                	current_fd = fd_table[i][1];
                	del_id_from_table(0, fd_table[i][0]);
                }
				else{
					current_fd = fd_table[i][1];
					/* получить сообщение в соответствии с его длиной */

					/* вызвать events(), передать параметры и сообщение */
					net_header = (struct network_msg_hdr_t *) buf;
					/* Получение индекса записи с ИД игрового сервера */
					return_val = get_index_by_fd(fd_table[i][1]);

					if(return_val < 0){
						printf("[game_server_network] get_index_by_fd() returned -1, cant find entry with id = %d\n", fd_table[i][0]);
						gameEvents(CLIENT, 0, net_header->payload_type, (void *) (buf + 8));
					}
					else
						gameEvents(CLIENT, return_val, net_header->payload_type, (void *) (buf + 8));
				}
			}
		}
	}
}

/*
 * Функция передачи сообщений.
 * destination_type: для GAME_SERVER, CLIENT
 * указать destination_id. В остальных случаях
 * destination_id = 0.
 */
void send_message(int destination_type, int destination_id,
                  int message_type, int message_len, void *message)
{
	struct network_msg_hdr_t *net_header;
	char buf[MSG_BUF_LEN];
	int return_val = 0;
	int i;
	int index;
/*
 * Формирование сообщения с добавлением
 * заголовка сетевого модуля.
 */
	memset(buf, 0 , MSG_BUF_LEN);
	net_header = (struct network_msg_hdr_t *) buf;
	net_header->payload_type = message_type;
	net_header->payload_len = message_len;
	memcpy(buf + sizeof(struct network_msg_hdr_t), message, message_len);

/*
 * Отправка сообщения в соответствии с
 * переданными параметрами назначения
 */
	switch(destination_type){
		case GAME_SERVER:
			index = get_index_by_id(destination_id);
			if(index < 0){
				printf("[network] get_index_by_fd() returned -1, cant find entry with id = %d\n", destination_id);
				printf("[network] message will not send\n");
				break;
			}

			return_val = write(fd_table[index][1], buf, MSG_BUF_LEN);

			if(return_val < 0){
				printf("[network] Game server with id = %d:\n", fd_table[index][0]);
				perror("[network] write()");
				printf("[network] Game server with id = %d: connection closed\n", fd_table[index][0]);
				del_id_from_table(0, fd_table[index][0]);
			}

			if(return_val == 0){
				printf("[network] Game server with id = %d:\n", fd_table[index][0]);
				printf("[network] write() returned 0, closing connection\n");
				printf("[network] Game server with id = %d: connection closed\n", fd_table[index][0]);
				del_id_from_table(0, fd_table[index][0]);
			}
			break;

		case LOBBY_SERVER:
			return_val = write(pipe_fd, buf, MSG_BUF_LEN);

			if(return_val < 0){
				printf("[network] Sending message to lobby server error:\n");
				perror("[network] write()");
				printf("[network] Connection with lobby server closed\n");
				if(close(pipe_fd) < 0){
					printf("[game_server_network] closing listen_server connection error:\n");
					perror("close()");
				}
				pipe_fd = 0;
			}

			if(return_val == 0){
				printf("[network] Sending message to lobby server error:\n");
				printf("[network] write() returned 0, closing connection\n");
				printf("[network] Connection with lobby server closed\n");
				if(close(pipe_fd) < 0){
					printf("[game_server_network] closing listen_server connection error:\n");
					perror("close()");
				}
				pipe_fd = 0;
			}
			break;

		case CLIENT:
			index = get_index_by_id(destination_id);
			if(index < 0){
				printf("[network] get_index_by_fd() returned -1, cant find entry with id = %d\n", destination_id);
				printf("[network] message will not send\n");
				break;
			}

			return_val = write(fd_table[index][1], buf, MSG_BUF_LEN);

			if(return_val < 0){
				printf("[network] Client with id = %d:\n", fd_table[index][1]);
				perror("[network] write()");
				printf("[network] Client with id = %d: connection closed\n", fd_table[index][1]);
				del_id_from_table(0, fd_table[index][0]);
			}

			if(return_val == 0){
				printf("[network] Client with id = %d:\n", fd_table[index][1]);
				printf("[network] write() returned 0, closing connection\n");
				printf("[network] Client with id = %d: connection closed\n", fd_table[index][1]);
				del_id_from_table(0, fd_table[index][0]);
			}
			break;

		case CURRENT:
			return_val = write(current_fd, buf, MSG_BUF_LEN);

			if(return_val < 0){
				printf("[network] Current client connection error: \n");
				perror("[network] write()");
				printf("[network] Current client connection error: connection closed\n");
				close_current_client_connection();
			}

			if(return_val == 0){
				printf("[network] Current client connection error: \n");
				printf("[network] write() returned 0, closing connection\n");
				printf("[network] Current client connection error: connection closed\n");
				close_current_client_connection();
			}
			break;

		case ALL_CLIENTS:
			for(i = 0; i < MAX_TABLE_LEN; i++){
				if(fd_table[i][0] != 0){
					return_val = write(fd_table[i][1], buf, MSG_BUF_LEN);

					if(return_val < 0){
						printf("[network] Client with id = %d:\n", fd_table[i][0]);
						perror("[network] write()");
						printf("[network] Client with id = %d: connection closed", fd_table[i][0]);
						del_id_from_table(0, fd_table[i][0]);
					}

					if(return_val == 0){
						printf("[network] Client with id = %d:\n", fd_table[i][0]);
						printf("[network] write() returned 0, closing connection\n");
						printf("[network] Client with id = %d: connection closed\n", fd_table[i][0]);
						del_id_from_table(0, fd_table[i][0]);
					}
				}
			}
			break;
		default:
			printf("[network] Send_message(): wrong destination argument\n");
			break;
	}
}

/* Закрытие текущего соединения с клиентом */
void close_current_client_connection(void)
{
	int i = 0;
	while((active_connection_socket[i] != current_fd) && (i < MAX_ACTIVE_CONNECTIONS))
		i++;
	if(i == MAX_ACTIVE_CONNECTIONS){
		printf("[network] close_current_client_connection(): cant found current_fd\n");
		printf("[network] close_current_client_connection(): connection not closed\n");
	}
	else{
		close(active_connection_socket[i]);
		active_connection_socket[i] = 0;
		connections_count--;
	}
}

/*
 * Добавление и удаление записи
 * в таблицу соответствия между id
 * и текущим файловым дескриптором.
 * При удалении записи закрывается
 * соединение. Если fd не известен
 * передавать 0.
 */
void add_id_to_table(int fd, int id)
{
	int i = get_index_by_id(0);	/* поиск свободной ячейки*/

	if(i < 0){	/* ошибка */
		printf("[network] add_id_to_table(): table overflowed or cant find entry with id = %d\n", id);
	}
	else{
		fd_table[i][0] = id;
		fd_table[i][1] = fd == 0 ? current_fd : fd;
	}
}

void del_id_from_table(int fd, int id)
{
	int index;
	if(fd == 0)
		index = get_index_by_id(id);
	else
		index = get_index_by_fd(fd);

	if(index < 0){	/* ошибка */
		printf("[network] del_id_to_table(): cant find entry with id = %d\n", id);
	}
	else{
		if(close(fd_table[index][1]) < 0){
			printf("[network] del_id_from_table(): closing connection error:\n");
			perror("close()");
		}

		fd_table[index][0] = 0;
		fd_table[index][1] = 0;
	}
}
