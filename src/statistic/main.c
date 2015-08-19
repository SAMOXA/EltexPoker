#include "stat_header.h"

int main(void)
{
    struct stat_list *players_list;

    char name[] = "Test Player";
    char name1[] = "FFF AAA";
    char name2[] = "BBB VVV";

    players_list = stat_init(players_list);
/*
    players_list = stat_create(players_list, name);


    players_list = stat_create(players_list, name1);
    players_list = stat_create(players_list, name2);

    stat_upd_win (players_list, name, 50, 2);
    stat_upd_lose(players_list, name, 40);
    stat_upd_lose(players_list, name, 40);
    stat_upd_lose(players_list, name, 40);
    stat_upd_win (players_list, name, 5, 250);

    stat_upd_win (players_list, name1, 50, 2);
    stat_upd_lose(players_list, name1, 40);
    stat_upd_lose(players_list, name2, 40);
    stat_upd_lose(players_list, name2, 40);
    stat_upd_win (players_list, name2, 5, 250);


    stat_show(players_list, name);
    stat_show(players_list, name1);
    stat_show(players_list, name2);
*/
    stat_show_all(players_list);

//    stat_save(players_list);

//    stat_show_all(players_list);

    return 0;
}