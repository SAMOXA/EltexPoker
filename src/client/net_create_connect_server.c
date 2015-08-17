#include "net_header.h"

/*
* Create socket and connect to server
* return value good: fd, error: -1
*/
int net_create_connect_server(char *addres, int port)
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
