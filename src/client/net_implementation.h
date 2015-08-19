#ifndef NET_IMPLEMENTATION_H
#define NET_IMPLEMENTATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>

int net_connect_server(char *addres, int port);
void net_disconnect_server(int fd);

int net_create_socket(int fd);
struct sockaddr_in net_fill_socket_addr(struct sockaddr_in addr, 
						char *addres,
						int port);
void net_connect(int fd, struct sockaddr_in server_addr);
int net_send(int fd, void *buffer, int size);
void *net_recive(int fd, void *buffer, int size);

#endif