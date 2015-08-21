#include "graf_api.h"
#include <unistd.h>
#include <string.h>

int main()
{
    int index=0;
    struct graf_list_t list;

    strcpy(list.title,"Tables list");
    for(index=0;index<10;index++){
	list.tables[index].enabled=1;
	list.tables[index].id=index+7;
	list.tables[index].players_count=4;
    }

    grafInitList();
    grafDrawTableList(&list);
    sleep(60);
    grafExitList();

    return 0;
}