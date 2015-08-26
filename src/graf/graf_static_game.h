#ifndef GRAF_H
#define GRAF_H

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <locale.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include "graf_api.h"

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
    int enabled;
    int last_enabled;
    int selected;
    char name[20];
    unsigned int pos[2];
    WINDOW *wnd;
};

struct ncs_graf_player_t{
    int enabled;
    int last_enabled;
    int selected;
    char name[GRAF_MAX_NAME_SIZE];
    char money_text[GRAF_MAX_MONEY_TEXT_SIZE];
    char status_text[GRAF_MAX_STATUS_TEXT_SIZE];
    struct ncs_graf_card_t cards[GRAF_CARDS_PER_PLAYER_COUNT];
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
    struct ncs_graf_card_t cards[GRAF_CARDS_ON_TABLE_COUNT];
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
	double top_player_size[2];
	double side_player_size[2];
	double bank_size[2];

	int text_color;
	int back_color;

	int player_text_color;
	int player_back_color;

	int sel_player_text_color;
	int sel_player_back_color;

	int card_text_color;
	int card_back_color;

	int sel_card_text_color;
	int sel_card_back_color;

	int bank_text_color;
	int bank_back_color;

	int btn_text_color;
	int btn_back_color;

	struct ncs_graf_player_t players[GRAF_MAX_PLAYERS];
	struct ncs_graf_bank_t bank;
	struct ncs_graf_input_t input;
	struct ncs_graf_button_t exit_btn;
	struct ncs_graf_button_t pass_btn;
	WINDOW *msg_wnd;
	unsigned int msg_pos[2];
	unsigned int msg_size[2];
	unsigned int card_size[2];
};

int ncsSetColor(WINDOW *wnd,unsigned int TextColor,unsigned int BackColor);
int ncsSetWndColor(WINDOW *wnd,unsigned int TextColor,unsigned int BackColor);
void ncsPrintInWnd(WINDOW* wnd,const unsigned int *pos,const char *text);
void ncsInitColorPairs();
void ncsResize(int signo);
void ncsInitMainTable(struct graf_table_t *tbl);
void ncsInitGame(struct graf_table_t *tbl);//инициализация
void ncsSetGame(struct graf_table_t *tbl);
void ncsClearPrint(WINDOW* wnd,unsigned int pos[2],char *text, int len);
void ncsShow();
#endif
