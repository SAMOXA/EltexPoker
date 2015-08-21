#ifndef LOGIC_CL_H
#define LOGIC_CL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "global.h"
#include <unistd.h>

extern int session;

int lobbyServer();
void gameServer();
//void startGraphicsLoop();

#endif
