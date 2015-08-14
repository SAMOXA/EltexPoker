#include "net_implementation.h"

int net_connect_server(char *addres, int port)
{
    static int 			fd;
    static struct sockaddr_in	server_addr;

    fd 		= net_create_socket(fd);
    server_addr = net_fill_socket_addr(server_addr, addres, port);
    net_connect(fd, server_addr);

    return fd;
}

void net_disconnect_server(int fd)
{
    close(fd);
}

void net_connect(int fd, struct sockaddr_in server_addr)
{
    // connect to the server
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	fprintf(stderr, "\033[0;31merror\033[0m: connect()\n");
	exit(1);
    }
}

int net_create_socket(int fd)
{
    // create TCP socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
	fprintf(stderr, "\033[0;31error\033[0m: socket()\n");
	exit(1);
    }
    return fd;
}

struct sockaddr_in net_fill_socket_addr(struct sockaddr_in addr, char *address, int port)
{
	// fill socket structure
	bzero((char*)&addr, sizeof(addr));

	addr.sin_family 	= AF_INET;
	addr.sin_addr.s_addr 	= inet_addr(address);
	addr.sin_port 		= htons(port);

	return addr;
}

int net_send(int fd, void *buffer, int size)
{
    int rw_bytes;

    rw_bytes = write(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
	fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
	//exit(1);
    }
    return rw_bytes;
}

void *net_recive(int fd, void *buffer, int size)
{
    int rw_bytes;

    rw_bytes = write(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
	fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
	//exit(1);
    }
    return buffer;
}