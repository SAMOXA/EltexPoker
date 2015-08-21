#ifndef GRAF_H
#define GRAF_H

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <termios.h>
#include <ncurses.h>
#include <locale.h>
#include <pthread.h>

#include "graf_api.h"

#define NCS_GRAF_CARD_SIZE_Y 4
#define NCS_GRAF_CARD_SIZE_X 4

#define NCS_GRAF_TABLE_SIZE_Y 24
#define NCS_GRAF_TABLE_SIZE_X 80

#define NCS_GRAF_CARD_NONE_COLOR COLOR_BLUE

#define NCS_GRAF_INPUT_TITLE_SIZE 128
#define NCS_GRAF_INPUT_BUFFER_SIZE 128
#define NCS_GRAF_BUTTON_TITLE_SIZE 128

#define NCS_GRAF_TABLE_BOTTOM 0
#define NCS_GRAF_TABLE_LEFT 1
#define NCS_GRAF_TABLE_TOP 2
#define NCS_GRAF_TABLE_RIGHT 3

#define NCS_GRAF_SPADES "♠"//пики
#define NCS_GRAF_CLUBS "♣"//трефы
#define NCS_GRAF_HEARTS "♥"//червы
#define NCS_GRAF_DIAMONDS "♦"//бубны

struct ncs_graf_card_t{
    int color;
    int selected;
    char name[20];
    unsigned int pos[2];
    WINDOW *wnd;
};

struct ncs_graf_player_t{
    int enabled;
    int selected;
    char name[GRAF_MAX_NAME_SIZE];
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char status_text[GRAF_MAX_STATUS_TEXT_SIZE];
    unsigned int card_num;
    struct ncs_graf_card_t *cards;
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int name_pos[2];
    unsigned int status_pos[2];
    unsigned int money_pos[2];
    unsigned int cards_pos[2];
    WINDOW *wnd;
};

struct ncs_graf_bank_t{
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char timer_text[GRAF_MAX_TIMER_TEXT_SIZE];
    unsigned int card_num;
    struct ncs_graf_card_t *cards;
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int money_pos[2];
    unsigned int cards_pos[2];
    unsigned int timer_pos[2];
    WINDOW *wnd;
};

struct ncs_graf_input_t{
    int enabled;
    int selected;
    char title[NCS_GRAF_INPUT_TITLE_SIZE];
    char buffer[NCS_GRAF_INPUT_BUFFER_SIZE];
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int title_pos[2];
    unsigned int buffer_pos[2];
    WINDOW *wnd;
};

struct ncs_graf_button_t{
    int enabled;
    int selected;
    char title[NCS_GRAF_BUTTON_TITLE_SIZE];
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int title_pos[2];
    WINDOW *wnd;
};

struct ncs_graf_table_t{
    struct ncs_graf_player_t** players;
    struct ncs_graf_bank_t bank;
    struct ncs_graf_input_t input;
    struct ncs_graf_button_t exit_btn;
    struct ncs_graf_button_t pass_btn;
    WINDOW *msg_wnd;
    unsigned int msg_pos[2];
    unsigned int msg_size[2];
    unsigned int card_size[2];
};

void ncs_grafAutoInit(	struct graf_table_t*);

void ncsGrafInitTable(	struct ncs_graf_table_t*,const struct graf_bank_t*,\
			const int,const int);
void ncsGrafInitPlayer(const struct ncs_graf_table_t*,\
		    int,\
		    int,const char*,\
		    const char*,\
		    const char*,\
		    int,int,int);
void ncsGrafSetCard(const struct ncs_graf_player_t*,\
		int,int,const char*,const char*,\
		int);
void ncsGrafSetBankCard(const struct ncs_graf_bank_t*,\
		int,int,const char*,const char*,\
		int);
void ncsStartGraf(struct ncs_graf_table_t*);
void ncsInitColorPairs();
int ncsSetColor(unsigned int,unsigned int);
int ncsSetWndColor(WINDOW*,unsigned int,unsigned int);
void ncsPrintInWnd(WINDOW*,const unsigned int*,const char*);
void ncsShow(const struct ncs_graf_table_t*);
void ncsShowBank(const struct ncs_graf_table_t*);
void ncsShowInput(const struct ncs_graf_table_t*);
void ncsShowBtn(const struct ncs_graf_button_t*);
void ncsEndGraf();

void* ncsControlsFunc(void*);

void ncsTempExit();
void ncsTempBet(int sum);
void ncsTempPass();

#endif