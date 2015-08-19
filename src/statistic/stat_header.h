#ifndef STAT_HEADER_H
#define STAT_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_BASE "server_statistic.txt"
#define DATA_BACKUP "backup_statistic.txt"

struct stat_node {
    char 	name[256];
    int 	total_game;
    int 	win;
    int 	lose;
    int 	spend_money;
    int 	win_money;
    float 	stat;

    struct stat_node *next;
};

struct stat_list {
    struct stat_node *head;
};

void stat_save(struct stat_list *list);
struct stat_list *stat_load(struct stat_list *list);

struct stat_list *stat_init(struct stat_list *list);
struct stat_list *stat_create(struct stat_list *list, char *name);
struct stat_node *stat_search(struct stat_list *list, char *name);

void stat_upd_win(struct stat_list *list, char *name, int s_money, int w_money);
void stat_upd_lose(struct stat_list *list, char *name, int s_money);
void stat_win(struct stat_node *my_stat, int s_money, int w_money);
void stat_lose(struct stat_node *my_stat, int s_money);
void stat_show(struct stat_list *list, char *name);
void stat_print(struct stat_node *player);

void stat_show_all(struct stat_list *list);

#endif