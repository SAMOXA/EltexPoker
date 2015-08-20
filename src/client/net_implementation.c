#include "net_implementation.h"

int net_connect_server(char *addres, int port)
{
    int fd;                         //server file descriptor
    struct sockaddr_in server_addr; //server structure

    /* Fill server structure */
    server_addr = net_fill_socket_addr(server_addr, addres, port);

    /* Create TCP socket */
    fd = net_create_socket(fd);
    if (fd < 0) {
	return -1;
    }

    /* Connect to server */
    if ((net_connect(fd, server_addr)) < 0) {
	return -1;
    }
    return fd;
}

struct sockaddr_in net_fill_socket_addr(struct sockaddr_in addr, char *address, int port)
{
    /* Zero out structure */
    bzero((char*)&addr, sizeof(addr));

    addr.sin_family      = AF_INET;            // setup family
    addr.sin_addr.s_addr = inet_addr(address); // setup addres
    addr.sin_port        = htons(port);        // setup port

    return addr;
}

int net_create_socket(int *fd)
{
    /* Create TCP socket */
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
	//fprintf(stderr, "\033[0;31error\033[0m: socket()\n");
	//exit(1);
	return -1;
    }
    return *fd;
}

int net_connect(int fd, struct sockaddr_in server_addr)
{
    /* Connect to server */
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	//fprintf(stderr, "\033[0;31merror\033[0m: connect()\n");
	//exit(1);
	return -1;
    }
    return 0;
}

void net_disconnect_server(int fd)
{
    close(fd);
}

int net_send(int fd, void *buffer, int size)
{
    /* Send buffer to server */
    int rw_bytes = write(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
	//fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
	//exit(1);
	return -1;
    }
    return rw_bytes;
}

void *net_receive(int fd, void *buffer, int size)
{
    /* Receive buffer from server */
    int rw_bytes = read(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
	//fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
	//exit(1);
	return NULL;
    }
    return buffer;
}