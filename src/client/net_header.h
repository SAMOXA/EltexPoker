#ifndef NET_HEADER_H
#define NET_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <pthread.h>

int net_create_socket(int fd);
int net_create_connect_server(char *addres, int port);
int net_send(int fd, void *buffer, int size);
int net_connect(int fd, struct sockaddr_in server_addr);
void *handler_send(void *args);
void *handler_recv(void *args);
void *net_receive(int fd, int size);
void client(void);
void net_disconnect_server(int fd);
struct sockaddr_in net_fill_socket_addr(struct sockaddr_in addr, 
						char *address, int port);

#endif