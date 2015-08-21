#ifndef NET_HEADER_H
#define NET_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <error.h>
#include <unistd.h>

#include "global.h"

extern int port;
extern char address[INET_ADDRSTRLEN];

int initNetwork();
int sendMsg(int type, int length, void *buf);
void* recvMsg(int *type);
int startNetworkThread();
void networkDisconnect();

#endif