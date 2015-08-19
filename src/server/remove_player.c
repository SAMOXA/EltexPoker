#include "logic.h"
/*
* Prototype of this function not added in logic.h or some other headers
*/

void removePlayer(int id)
{
    int i, index;
    int max_players = MAX_TABLES_COUNT * MAX_PLAYERS_PER_TABLE;
    char remove_name[256];

    /* 1) Searching player's name by id */
    for (index = 0; i < max_players; i++) {
	if (IdName[index].id == id) {
	    /* If name found */
	    strcpy(remove_name, IdName[index].name);
	    break;
	}
    }

    /* 2) Searching requested player by name */
    for (index = 0; index < MAX_TABLES_COUNT; index++) {
	for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) {
	    if ((strcmp(lists[index].tables[i], remove_name)) == 0) {
		/* If name found */
		bzero(lists[index].tables[i], MAX_NAME_LENGTH);
		/* 3) Decrement structure "information of table" */
		inofList[index].countPlayer -= 1;
		return;
	    }
	}
    }
}
