#include "net_header.h"

int socket_fd;

void client(void)
{
	static int 					new_port	= 0;
	static int 					port 		= 5555;
	static int					rw_bytes 	= 0;

	static char 				address[] 	= "192.168.0.5";
	static char					buffer[256];

	printf("--\033[4;34mTesting client\033[0m--\n");

	socket_fd = net_create_connect_server(address, port);
	if (socket_fd < 0) {
		fprintf(stderr, "\033[0;31m-Error\033[0m: connect(main server)\n");
		exit(1);
	}

	bzero(buffer, 256);
	printf("--\033[0;32mConnecting\033[0m to server: %s:%d\n", address, port);
	strcpy(buffer, "connect");

	printf("---%d\n", socket_fd);

	pthread_t th_recv, th_send;

	if ((pthread_create(&th_recv, NULL, handler_recv, (void*)&socket_fd)) != 0) {
		fprintf(stderr, "\033[0;31m-Error\033[0m: pthread_create(recv)\n");
	}

	if ((pthread_create(&th_send, NULL, handler_send, (void*)&socket_fd)) != 0) {
		fprintf(stderr, "\033[0;31m-Error\033[0m: pthread_create(send)\n");
	}

	pthread_join(th_send, NULL);
	pthread_join(th_recv, NULL);

	net_disconnect_server(socket_fd);
}
