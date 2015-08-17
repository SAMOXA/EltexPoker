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

/*
    // send message
    rw_bytes = net_send(socket_fd, buffer, 256);
    if (rw_bytes < 0) {
		fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
		exit(1);
	}

	// read response
	bzero(buffer, 256);

	char *ptr = NULL;

	ptr = net_receive(socket_fd, 256);
	if (!ptr) {
		fprintf(stderr,"\033[0;31merror\033[0m: read()\n");
		exit(1);	
	}

	printf("Ready game tables:\n");
	printf("%s\nChoose: ", ptr);
	char input[256];
	scanf("%s", input);
	strcpy(buffer, input);

	net_disconnect_server(socket_fd);
	
	socket_fd = net_create_connect_server(address, port);
	if (socket_fd < 0) {
		fprintf(stderr, "\033[0;31m-Error\033[0m: connect(main server#2)\n");
		exit(1);	
	}

	// send message
    rw_bytes = net_send(socket_fd, buffer, 256);
    if (rw_bytes < 0) {
		fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
		exit(1);
	}
*/
	net_disconnect_server(socket_fd);
}