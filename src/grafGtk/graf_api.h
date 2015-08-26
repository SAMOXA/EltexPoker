#ifndef GRAF_API_H
#define GRAF_API_H

#include "global.h"

#define GRAF_MAX_PLAYERS 4

//------------------------Game API

#define GRAF_MAX_NAME_SIZE 128
#define GRAF_MAX_MONEY_TEXT_SIZE 128
#define GRAF_MAX_STATUS_TEXT_SIZE 128
#define GRAF_MAX_TIMER_TEXT_SIZE 128

#define GRAF_INDEX_NONE -1//отсутствие масти
#define GRAF_INDEX_SPADES 0//пики
#define GRAF_INDEX_CLUBS 1//трефы
#define GRAF_INDEX_HEARTS 2//червы
#define GRAF_INDEX_DIAMONDS 3//бубны

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
    struct graf_card_t cards[2];
};

struct graf_bank_t{
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char timer_text[GRAF_MAX_TIMER_TEXT_SIZE];
    struct graf_card_t cards[5];
};

struct graf_table_t{
    struct graf_player_t players[GRAF_MAX_PLAYERS];
    struct graf_bank_t bank; 
};

void grafInit(struct graf_table_t*);
void grafDrawAll(struct graf_table_t*);
void grafDrawUserMsg(const char* msg);
void grafExit();

void grafShowInput(const char* title, const char* default_text);
void grafHideInput();

extern void (*graf_exit_event)(void);
extern void (*graf_bet_event)(int sum);
extern void (*graf_pass_event)(void);

//---------------------------Table select API

struct graf_list_t{
    char title[256];
    struct table_t tables[MAX_TABLES_COUNT];
};

void grafInitList(struct graf_list_t* list);
void grafDrawList(struct graf_list_t* list);
void grafDrawListMsg(const char *msg);

extern void (*graf_list_exit_event)(void);
extern void (*graf_list_select_event)(int id);
extern void (*graf_list_create_event)(void);
extern void (*graf_list_refresh_event)(void);


#endif
