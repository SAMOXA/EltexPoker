#ifndef GRAF_LIST_H
#define GRAF_LIST_H

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <termios.h>
#include <ncurses.h>
#include <locale.h>
#include <pthread.h>

#include "graf.h"
#include "graf_api.h"

/*struct ncs_graf_button_t{
    int enabled;
    int selected;
    char title[NCS_GRAF_BUTTON_TITLE_SIZE];
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int title_pos[2];
    WINDOW *wnd;
};*/

struct ncs_graf_list_t{
    int enabled;
    int selected;
    char title[256];
    struct graf_list_table_t tables[MAX_TABLES_COUNT];
    int selected_index;
    struct ncs_graf_button_t exit_btn;
    struct ncs_graf_button_t create_btn;
    struct ncs_graf_button_t refresh_btn;

    unsigned int pos[2];
    unsigned int size[2];

    WINDOW *wnd;
};

void ncsListGrafInit(	struct ncs_graf_list_t*,const struct graf_list_t*);
void ncsListStartGraf(	struct ncs_graf_list_t*);
//void ncsInitColorPairs();
//int ncsSetColor(unsigned int,unsigned int);
//int ncsSetWndColor(WINDOW*,unsigned int,unsigned int);
//void ncsPrintInWnd(WINDOW*,const unsigned int*,const char*);
void ncsListShow(const struct ncs_graf_list_t*);
//void ncsShowBtn(const struct ncs_graf_button_t*);
void ncsListEndGraf();

void* ncsListControlsFunc(void*);

void ncsTempListExit();
void ncsTempListSelect(int id);
void ncsTempListCreate();
void ncsTempListRefresh();

#endif