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

void newEvent(unsigned int id, unsigned char type, char sourceType, void *data);
void initGameLogic();

#endif //GAMELOGIC_H
