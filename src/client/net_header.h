#ifndef NET_HEADER_H
#define NET_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../global.h"


char net_error_msg[256];

int net_disconnect_server();
int net_create_connect_server(char *addres, int port);

int net_send(void *buffer, int type, int len);
int net_receive(void *buffer, int *type, int *len);

#endif