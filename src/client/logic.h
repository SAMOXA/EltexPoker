#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "global.h"
#include <unistd.h>
#ifdef HAVE_NCURSES
#include <pthread.h>
#endif

extern int session;
extern int myId;

int lobbyServer();
void gameServer();
#ifdef HAVE_NCURSES
void startGraphicsWaitLoop();
#endif

#endif
