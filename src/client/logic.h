#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "global.h"
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#ifdef HAVE_NCURSES

#endif

extern int session;
extern int myId;

int lobbyServer();
void gameServer();
void gameHandler(int type, void *msg);
#ifdef HAVE_NCURSES
void startGraphicsWaitLoop();
#endif

#endif
