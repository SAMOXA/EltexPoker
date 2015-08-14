#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <string.h>

#include "../../global.h"
#include "../internalIPC.h"

void newEvent(unsigned int id, unsigned char type, char sourceType, void *data);
void initGameLogic();

#endif //GAMELOGIC_H
