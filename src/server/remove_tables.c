#include "logic.h"
/*
* Prototype of this function not added in logic.h or some other headers
*/

void removeTable(int id)
{
	int i, index;

	/* Searching requested table by id */
	for (index = 0; index < MAX_TABLES_COUNT; index++) {
		if (id == lists[index].id) {
			break;
		}
	}
	/* If table found */

	/* 1) Clean up structure "table" */
	for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) {
		bzero(lists[index].tables[i], MAX_NAME_LENGTH);
	}

	/* 2) Clean up structure "information of table" */
	inofList[index].status 	= EMPTY; 
	inofList[index].countPlayer = 0;
}
