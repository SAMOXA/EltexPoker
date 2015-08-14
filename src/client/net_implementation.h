#ifndef NET_IMPLEMENTATION_H
#define NET_IMPLEMENTATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
* Create TCP scoket and connect to server 
* return value: good "< 0", error "-1"
*/
int net_connect_server (char *addres, int port);

/*
* Create TCP socket
* return value: good "< 0", error "-1"
*/
int net_create_socket (int fd);

/*
* Connect to server 
* return value: good "< 0", error "-1"
*/
int net_connect (int fd, struct sockaddr_in server_addr);

/* 
* Write message 
* return value: good "< 0", error "-1"
*/
int net_send (int fd, void *buffer, int size);

/* Disconnect from server */
void net_disconnect_server (int fd);

/*
* Read message
* return value: good !NULL, error NULL
*/
void *net_receive (int fd, void *buffer, int size);

/*
* Fill server structure 
* return value: struct  sockaddr_in
*/
struct sockaddr_in net_fill_socket_addr (struct sockaddr_in addr, char *addres, int port);

#endif