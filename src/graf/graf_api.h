#ifndef GRAF_API_H
#define GRAF_API_H

#include "../global.h"

#define GRAF_MAX_PLAYERS 4

//------------------------Game API

#define GRAF_MAX_NAME_SIZE 128
#define GRAF_MAX_MONEY_TEXT_SIZE 128
#define GRAF_MAX_STATUS_TEXT_SIZE 128
#define GRAF_MAX_TIMER_TEXT_SIZE 128

#define GRAF_INDEX_NONE -1//отсутствие масти
#define GRAF_INDEX_HEARTS 0//пики
#define GRAF_INDEX_DIAMONDS 1//трефы
#define GRAF_INDEX_CLUBS 2//червы
#define GRAF_INDEX_SPADES 3//бубны

#define CARD_COUNT_PER_PLAYER 2 // It hold'em, we also can omaha
#define CARD_COUNT_FOR_BANK 5 // hold'em, not omaha

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
    unsigned int card_num;
    struct graf_card_t cards[CARD_COUNT_PER_PLAYER];
//    unsigned int index_pos;//0 - пользователь
};

struct graf_bank_t{
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char timer_text[GRAF_MAX_TIMER_TEXT_SIZE];
    unsigned int card_num;
    struct graf_card_t cards[CARD_COUNT_PER_PLAYER];
};

struct graf_table_t{
    struct graf_player_t players[GRAF_MAX_PLAYERS];
    struct graf_bank_t bank;
};

//void grafInit(struct graf_table_t*);//без отрисовки
void grafDrawAll(struct graf_table_t*);
void grafDrawUserMsg(const char* msg);//вывод строки состояния
//void grafDrawPlayer(int pos);//struct graf_player_t*,
//void grafDrawBank(struct graf_bank_t*);
//void grafDrawTimer(const char* timer);
//void grafSetPlayer(struct graf_player_t*,int pos);
//void grafSetBank(struct graf_bank_t*);
void grafExit();

void grafShowInput(const char* title,const char* default_text);
void grafHideInput();

extern void (*graf_exit_event)(void);
extern void (*graf_bet_event)(int sum);
extern void (*graf_pass_event)(void);

//---------------------------Table select API

struct graf_list_table_t{
    int enabled;
    int id;
    int players_count;
};

struct graf_list_t{
    char title[256];
    struct graf_list_table_t tables[MAX_TABLES_COUNT];
};

void grafInitList();
void grafDrawTableList(struct graf_list_t*);
void grafDrawMsgList(const char *msg);//вывод строки состояния
void grafExitList();

extern void (*graf_list_exit_event)(void);
extern void (*graf_list_select_event)(int id);
extern void (*graf_list_create_event)(void);
extern void (*graf_list_refresh_event)(void);


#endif
