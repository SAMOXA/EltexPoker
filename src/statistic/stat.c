#include "stat_header.h"

/* Initialize statistic list */
struct stat_list *stat_init(struct stat_list *list)
{
    FILE *data_base;

    list = malloc(sizeof(struct stat_list));
    if (!list) {
        fprintf(stderr, "error: malloc(stat_init)\n");
        return NULL;
    }

    list->head = NULL;

    if ((data_base = fopen(DATA_BASE, "r")) != NULL) {
	list = stat_load(list);
	fclose(data_base);
    }
    return list;
}

/* Create new player in list */
struct stat_list *stat_create(struct stat_list *list, char *name)
{
    struct stat_node *new_player, *old_head;

    new_player = malloc(sizeof(struct stat_node));
    if (!new_player) {
	fprintf(stderr, "error: malloc(stat_create)\n");
	return;
    }

    strcpy(new_player->name, name);
    new_player->total_game 	= 0;
    new_player->win 		= 0;
    new_player->lose 		= 0;
    new_player->spend_money 	= 0;
    new_player->win_money 	= 0;
    new_player->stat 		= 0;
    new_player->next 		= NULL;

    if (list->head == NULL) {
	list->head = new_player;
    } else {
	old_head = list->head;
	list->head = new_player;
	new_player->next = old_head;
    }

    return list;
}

/* Save Statistic to file */
void stat_save(struct stat_list *list)
{
    FILE *data_base;

    struct stat_node *save_node;

    if (list->head == NULL) {
	return;
    }

    save_node = list->head;

    if ((data_base = fopen(DATA_BASE, "r")) != NULL) {
	printf("rewrite data\n");
	char command[256];
	strcpy(command, "rm ");
	strcat(command, DATA_BASE);
	system(command);
	fclose(data_base);
    }

    data_base = fopen(DATA_BASE, "a");
    if (!data_base) {
	fprintf(stderr, "error: fopen(DATA_BASE)\n");
	return;
    }

    while(save_node != NULL) {
	fwrite(save_node, sizeof(struct stat_node), 1, data_base);
	save_node = save_node->next;
    }
    fclose(data_base);
}

/* Load Statistic from file */
struct stat_list *stat_load(struct stat_list *list)
{
    FILE *data_base;

    struct stat_node *old_head;

    data_base = fopen(DATA_BASE, "r");
    if (!data_base) {
	fprintf(stderr, "error: fopen(stat_load)\n");
	return;
    }

    while(!feof(data_base)) {
        struct stat_node *load_node;

        load_node = malloc(sizeof(struct stat_node));

	if (!load_node) {
	    fprintf(stderr, "error: malloc(stat_load)\n");
	    exit(1);
	}

	if((fread(load_node, sizeof(struct stat_node), 1, data_base)) != 1) {
	    break;
	}

	//stat_print(&print_node);

	// for fist time old_head will be NULL ptr
	old_head = list->head;
	load_node->next = old_head;
	list->head = load_node;
    }

    fclose(data_base);
    return list;
}

/* Search player in statistic list */
struct stat_node *stat_search(struct stat_list *list, char *name)
{
    struct stat_node *search_player;

    if (list->head == NULL) {
	return NULL;
    }

    if (list->head->next == NULL) {
	if (strcmp(list->head->name, name) == 0) {
	    return list->head;
	} else {
	    return NULL;
	}
    }

    search_player = list->head;

    while(search_player != NULL) {
	if (strcmp(search_player->name, name) == 0) {
	    return search_player;
	}
	search_player = search_player->next;
    }

    return NULL;
}

/* Search player for update statistic if win game */
void stat_upd_win(struct stat_list *list, char *name, int s_money, int w_money)
{
    struct stat_node *search_player;

    search_player = stat_search(list, name);
    if (!search_player) {
	return;
    }
    stat_win(search_player, s_money, w_money);
}

/* Search player for update statistic if lose game */
void stat_upd_lose(struct stat_list *list, char *name, int s_money)
{
    struct stat_node *search_player;

    search_player = stat_search(list, name);
    if (!search_player) {
	return;
    }
    stat_lose(search_player, s_money);
}

/* Update statistic if win */
void stat_win(struct stat_node *player_stat, int s_money, int w_money)
{
    player_stat->total_game 	+= 1;
    player_stat->win 		+= 1;
    player_stat->spend_money 	+= s_money;
    player_stat->win_money 	+= w_money;
    player_stat->stat 		= ((double)player_stat->win/(double)player_stat->total_game)*100;

}

/* Update statistic if lose */
void stat_lose(struct stat_node *player_stat, int s_money)
{
    player_stat->total_game 	+= 1;
    player_stat->lose 		+= 1;
    player_stat->spend_money 	+= s_money;
    player_stat->stat 		= ((double)player_stat->win/(double)player_stat->total_game)*100;

}

/* Search player and show his statistic */
void stat_show(struct stat_list *list, char *name)
{
    struct stat_node *player;

    player = stat_search(list, name);
    if (!player) {
	return;
    }
    stat_print(player);
}

/* Show all statistic for each player */
void stat_show_all(struct stat_list *list)
{
    struct stat_node *print_node;

    print_node = list->head;

    while(print_node != NULL) {
	stat_print(print_node);
	print_node = print_node->next;
    }
}

/* Print statistic to the screen for the current player */
void stat_print(struct stat_node *my_stat)
{
    printf("\n+---------------------\n");
    printf("|-\033[0;34mPlayer name: %s\033[0m\n", my_stat->name);
    printf("|-\033[0;34mTotal games: %d\033[0m\n", my_stat->total_game);
    printf("|-\033[0;34mStatistic  : %.1f%\033[0m\n", my_stat->stat);
    printf("|-\033[0;32mWin games  : %d\033[0m\n", my_stat->win);
    printf("|-\033[0;31mLose games : %d\033[0m\n", my_stat->lose);
    printf("|-\033[0;31mSpend money: %d\033[0m\n", my_stat->spend_money);
    printf("|-\033[0;32mWin money  : %d\033[0m\n", my_stat->win_money);
    printf("+----------------------\n");
}