#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "global.h"
#include "internalIPC.h"
#include "win.h"
#include "server_network.h"
#include "shared.h"

void gameEvents(int sourceType, int id, int type, void *data);
void initGameLogic();

#endif //GAMELOGIC_H
