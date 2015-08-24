#ifndef GRAF_API_H
#define GRAF_API_H

#include "../global.h"

#define GRAF_MAX_PLAYERS MAX_PLAYERS_PER_TABLE

//------------------------Game API

#define GRAF_MAX_NAME_SIZE 32
#define GRAF_MAX_MONEY_TEXT_SIZE 32
#define GRAF_MAX_STATUS_TEXT_SIZE 32
#define GRAF_MAX_TIMER_TEXT_SIZE 8

#define GRAF_INDEX_NONE -1//отсутствие масти
#define GRAF_INDEX_SPADES 0//пики
#define GRAF_INDEX_CLUBS 1//трефы
#define GRAF_INDEX_HEARTS 2//червы
#define GRAF_INDEX_DIAMONDS 3//бубны

#define GRAF_CARDS_ON_TABLE_COUNT 5
#define GRAF_CARDS_PER_PLAYER_COUNT 2

struct graf_card_t{
    int selected;
    char val[2];
    int index_suit;
};

struct graf_player_t{
    int enabled;
    int selected;
    char name[GRAF_MAX_NAME_SIZE];
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char status_text[GRAF_MAX_STATUS_TEXT_SIZE];
    struct graf_card_t cards[GRAF_CARDS_PER_PLAYER_COUNT];
};

struct graf_bank_t{
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char timer_text[GRAF_MAX_TIMER_TEXT_SIZE];
    struct graf_card_t cards[GRAF_CARDS_ON_TABLE_COUNT];
};

struct graf_table_t{
    struct graf_player_t players[GRAF_MAX_PLAYERS];
    struct graf_bank_t bank;
};

void grafDrawAll(struct graf_table_t*);//отобразить текущее состояние игры
void grafDrawUserMsg(const char* msg);//вывод строки состояния
void grafExit();//возврат терминала в первоначальное состояние

//отобразить окно ввода ставки
void grafShowInput(struct graf_table_t*,const char* title,const char* default_text);
void grafHideInput(struct graf_table_t*);//скрыть окно ввода ставки

extern void (*graf_exit_event)(void);
extern void (*graf_bet_event)(int sum);
extern void (*graf_pass_event)(void);

//---------------------------Table select API

#define GRAF_LIST_TITLE_MAX_SIZE 32 

struct graf_list_table_t{
    int enabled;
    int id;
    int players_count;
};

struct graf_list_t{
    char title[GRAF_LIST_TITLE_MAX_SIZE];
    struct graf_list_table_t tables[MAX_TABLES_COUNT];
};

void grafInitList();//инициализация списка столов
void grafDrawTableList(struct graf_list_t*);//отобразить список столов
void grafDrawMsgList(const char *msg);//вывод строки состояния
void grafExitList();//корректное восстановление терминала

extern void (*graf_list_exit_event)(void);
extern void (*graf_list_select_event)(int id);
extern void (*graf_list_create_event)(void);
extern void (*graf_list_refresh_event)(void);

#endif
