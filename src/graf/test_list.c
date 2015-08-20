#include "graf_api.h"
#include <unistd.h>
#include <string.h>

int main()
{
    int index=0;
    struct graf_list_t list;

    strcpy(list.title,"Tables list");
    for(index=0;index<MAX_TABLES_COUNT;index++){
	list.tables[index].enabled=1;
	list.tables[index].id=index;
	list.tables[index].players_count=4;
    }

    grafInitList();
    grafDrawTableList(&list);
    sleep(60);
    grafExitList();

    return 0;
}