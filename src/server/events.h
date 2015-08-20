#ifndef EVENTS_H
#define EVENTS_H

#include <stdio.h>

#include "global.h"
#include "internalIPC.h"
#include "logic.h"

void events(int dest_type, int id, int msg_type, void *buf);
void init();

#endif //EVENTS_H
