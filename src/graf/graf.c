#include "graf.h"

static struct termios stored_settings;

static unsigned int CUR_TEXT_COLOR=COLOR_BLACK;
static unsigned int CUR_BACK_COLOR=COLOR_WHITE;

static struct ncs_graf_table_t *main_tbl=NULL;

static selected_index=0;

void (*graf_exit_event)(void)=ncsTempExit;
void (*graf_bet_event)(int sum)=ncsTempBet;
void (*graf_pass_event)(void)=ncsTempPass;

//-------------------------------------API connect block
void ncsGrafDelPlayer(struct ncs_graf_player_t **player)
{
    int index=0;

    if((*player)==NULL){
	return;
    }

    for(index=0;index<(*player)->card_num;index++){
	if((*player)->cards[index].wnd!=NULL){
	    delwin((*player)->cards[index].wnd);
	    (*player)->cards[index].wnd=NULL;
	}
    }
    free((*player)->cards);

    if((*player)->wnd!=NULL){
	delwin((*player)->wnd);
	(*player)->wnd=NULL;
    }
    
    free((*player));
    (*player)=NULL;
}

void ncsGrafDelTable(struct ncs_graf_table_t **tbl)
{
    int index=0;

    if((*tbl)==NULL){
	return;
    }

//Free players
    for(index=0;index<GRAF_MAX_PLAYERS;index++){
	ncsGrafDelPlayer(&((*tbl)->players[index]));
    }

//Free bank cards
    for(index=0;index<(*tbl)->bank.card_num;index++){
	if((*tbl)->bank.cards[index].wnd!=NULL){
	    delwin((*tbl)->bank.cards[index].wnd);
	    (*tbl)->bank.cards[index].wnd=NULL;
	}
    }
    free((*tbl)->bank.cards);    
    delwin((*tbl)->bank.wnd);
    (*tbl)->bank.wnd=NULL;

    delwin((*tbl)->input.wnd);
    (*tbl)->input.wnd=NULL;

    delwin((*tbl)->exit_btn.wnd);
    (*tbl)->exit_btn.wnd=NULL;
    delwin((*tbl)->pass_btn.wnd);
    (*tbl)->pass_btn.wnd=NULL;

    free((*tbl));
    (*tbl)=NULL;
}

void ncsGrafImportPlayer(const struct graf_player_t* api_player,struct ncs_graf_player_t** player,int index)
{
    if((*player)!=NULL){
	ncsGrafDelPlayer(player);
    }

    ncsGrafInitPlayer(main_tbl,index,index,\
		    api_player->name,\
		    api_player->status_text,\
		    api_player->money_text,\
		    api_player->card_num,\
		    api_player->enabled);
}

void ncsGrafImportTable(const struct graf_table_t* api_tbl,\
			struct ncs_graf_table_t** tbl)
{
    int index=0;

    if((*tbl)!=NULL){
	ncsGrafDelTable(tbl);
    }

    (*tbl)=(struct ncs_graf_table_t*)malloc(sizeof(struct ncs_graf_table_t));

//Pre-initialisation table
    ncsGrafInitTable((*tbl),&(api_tbl->bank),NCS_GRAF_CARD_SIZE_Y,NCS_GRAF_CARD_SIZE_X);

//Loading players
    for(index=0;index<GRAF_MAX_PLAYERS;index++){
	(*tbl)->players[index]=NULL;
	ncsGrafInitPlayer((*tbl),index,index,\
		    api_tbl->players[index].name,\
		    api_tbl->players[index].status_text,\
		    api_tbl->players[index].money_text,\
		    api_tbl->players[index].card_num,\
		    api_tbl->players[index].enabled);

	//ncsGrafImportPlayer(&(api_tbl->players[index]),&((*tbl)->players[index]),index);
    }
}

//------------------------------------API block
void grafInit(struct graf_table_t* tbl)
{
    int index=0;
    pthread_t thread;

    ncsGrafImportTable(tbl,&main_tbl);
//    for(index)

//    printf("%s\n",main_tbl->bank.money_text);

    pthread_create(&thread,NULL,ncsControlsFunc,NULL);
    pthread_detach(thread);
    ncsStartGraf(main_tbl);
}

void grafDrawAll()
{
    int index=0;

    ncsShow(main_tbl);
    wrefresh(main_tbl->exit_btn.wnd);
    wrefresh(main_tbl->pass_btn.wnd);

    wrefresh(main_tbl->bank.wnd);
    for(index=0;index<GRAF_MAX_PLAYERS;index++){
	if(main_tbl->players[index]!=NULL){
	    wrefresh(main_tbl->players[index]->wnd);
	}
    }
    refresh();
//    getch();
}
void grafDrawUserMsg(const char* msg)
{
    int pos[2]={0,0};
    ncsPrintInWnd(stdscr,pos,msg);
}
void grafDrawPlayer(int pos){};
void grafDrawBank(struct graf_bank_t* t)
{
    ncsShowBank(main_tbl);
    wrefresh(main_tbl->bank.wnd);
    refresh();
}
void grafDrawTimer(const char* timer)
{
    strcpy(main_tbl->bank.timer_text,timer);
    ncsShowBank(main_tbl);
    wrefresh(main_tbl->bank.wnd);
    refresh();
}

void grafSetPlayer(struct graf_player_t* t,int pos){};
void grafSetBank(struct graf_bank_t* t){};
void grafExit()
{
    ncsEndGraf();
}

void grafShowInput(const char *title,const char *default_text)
{
    strcpy(main_tbl->input.title,title);
    strcpy(main_tbl->input.buffer,default_text);
    main_tbl->input.enabled=1;
    ncsShowInput(main_tbl);
    wrefresh(main_tbl->input.wnd);
    refresh();
}

void grafHideInput()
{
    int index=0;

    main_tbl->input.enabled=0;
    clear();
    ncsShow(main_tbl);
    wrefresh(main_tbl->exit_btn.wnd);
    wrefresh(main_tbl->pass_btn.wnd);

    wrefresh(main_tbl->bank.wnd);
    for(index=0;index<GRAF_MAX_PLAYERS;index++){
	if(main_tbl->players[index]!=NULL){
	    wrefresh(main_tbl->players[index]->wnd);
	}
    }
    refresh();
}

//---------------------------------Ncurses opereations block
void ncsGrafInitTable(	struct ncs_graf_table_t* tbl,\
			const struct graf_bank_t* api_bank,\
			const int card_size_y,\
			const int card_size_x)
{
    int index=0;

//тут проверка на правильность ввода данных

    tbl->card_size[0]=card_size_y;
    tbl->card_size[1]=card_size_x;

    strncpy(tbl->bank.timer_text,api_bank->timer_text,GRAF_MAX_TIMER_TEXT_SIZE-1);
    tbl->bank.timer_text[GRAF_MAX_TIMER_TEXT_SIZE-1]='\0';
    strncpy(tbl->bank.money_text,api_bank->money_text,GRAF_MAX_MONEY_TEXT_SIZE-1);
    tbl->bank.money_text[GRAF_MAX_MONEY_TEXT_SIZE-1]='\0';
    tbl->bank.cards=(struct ncs_graf_card_t*)malloc(sizeof(struct ncs_graf_card_t)*api_bank->card_num);

    tbl->bank.card_num=api_bank->card_num;

    for(index=0;index<api_bank->card_num;index++){
	int color=0;
	char suit[3]="";
	char val[2]="";
	switch(api_bank->cards[index].index_suit){
	    case GRAF_INDEX_NONE:	color=NCS_GRAF_CARD_NONE_COLOR; 
					break;
	    case GRAF_INDEX_SPADES:	strncpy(suit,NCS_GRAF_SPADES,3);
					color=COLOR_BLACK;
					break;
	    case GRAF_INDEX_CLUBS:	strncpy(suit,NCS_GRAF_CLUBS,3);
					color=COLOR_BLACK;
					break;
	    case GRAF_INDEX_HEARTS:	strncpy(suit,NCS_GRAF_HEARTS,3);
					color=COLOR_RED;
					break;
	    case GRAF_INDEX_DIAMONDS:	strncpy(suit,NCS_GRAF_DIAMONDS,3);
					color=COLOR_RED;
					break;
	}
	val[0]=api_bank->cards[index].val;
	val[1]='\0';
	ncsGrafSetBankCard(&(tbl->bank),index,color,val,suit,api_bank->cards[index].selected);
    }

    tbl->bank.size[0]=6+2;
    tbl->bank.size[1]=30+2;
    tbl->bank.pos[0]=(NCS_GRAF_TABLE_SIZE_Y-6-2)/2;
    tbl->bank.pos[1]=(NCS_GRAF_TABLE_SIZE_X-30-2)/2;
    tbl->bank.money_pos[0]=1;
    tbl->bank.money_pos[1]=1;
    tbl->bank.timer_pos[0]=1;
    tbl->bank.timer_pos[1]=30+1-strlen(tbl->bank.timer_text);
    tbl->bank.cards_pos[0]=3;
    tbl->bank.cards_pos[1]=1;

    tbl->input.enabled=0;
    tbl->input.selected=0;
    tbl->input.title[0]='\0';
    tbl->input.buffer[0]='\0';

    tbl->input.size[0]=5+2;
    tbl->input.size[1]=20+2;
    tbl->input.pos[0]=NCS_GRAF_TABLE_SIZE_Y-5-2;
    tbl->input.pos[1]=0;
    tbl->input.title_pos[0]=1;
    tbl->input.title_pos[1]=1;
    tbl->input.buffer_pos[0]=2;
    tbl->input.buffer_pos[1]=1;

    tbl->exit_btn.enabled=1;
    tbl->exit_btn.selected=1;
    strcpy(tbl->exit_btn.title,"EXIT");

    tbl->exit_btn.size[0]=1+2;
    tbl->exit_btn.size[1]=20+2;
    tbl->exit_btn.pos[0]=NCS_GRAF_TABLE_SIZE_Y-1-2;
    tbl->exit_btn.pos[1]=NCS_GRAF_TABLE_SIZE_X-20-2;
    tbl->exit_btn.title_pos[0]=1;
    tbl->exit_btn.title_pos[1]=1;

    tbl->pass_btn.enabled=1;
    tbl->pass_btn.selected=0;
    strcpy(tbl->pass_btn.title,"PASS");

    tbl->pass_btn.size[0]=1+2;
    tbl->pass_btn.size[1]=20+2;
    tbl->pass_btn.pos[0]=NCS_GRAF_TABLE_SIZE_Y-1-2-3;
    tbl->pass_btn.pos[1]=NCS_GRAF_TABLE_SIZE_X-20-2;
    tbl->pass_btn.title_pos[0]=1;
    tbl->pass_btn.title_pos[1]=1;

    tbl->players=(struct ncs_graf_player_t**)(malloc(sizeof(struct ncs_graf_player_t*)*GRAF_MAX_PLAYERS));
    for(index=0;index<GRAF_MAX_PLAYERS;index++){
	tbl->players[index]=NULL;
    }

    return;
}

void ncsGrafInitPlayer(const struct ncs_graf_table_t* tbl,\
		    int player_index,\
		    int pos,const char* name,\
		    const char* status,\
		    const char* money,\
		    int card_num,\
		    int enabled)
{
    int index=0;
    struct ncs_graf_player_t* player=tbl->players[player_index];
//тут проверка на правильность ввода данных

    if(player==NULL){
	player=(struct ncs_graf_player_t*)malloc(sizeof(struct ncs_graf_player_t));
	tbl->players[player_index]=player;
    }
    strncpy(player->name,name,GRAF_MAX_NAME_SIZE-1);
    player->name[GRAF_MAX_NAME_SIZE-1]='\0';
    strncpy(player->status_text,status,GRAF_MAX_STATUS_TEXT_SIZE-1);
    player->status_text[GRAF_MAX_STATUS_TEXT_SIZE-1]='\0';
    strncpy(player->money_text,money,GRAF_MAX_MONEY_TEXT_SIZE-1);
    player->money_text[GRAF_MAX_MONEY_TEXT_SIZE-1]='\0';
    player->cards=(struct ncs_graf_card_t*)malloc(sizeof(struct ncs_graf_card_t)*card_num);

    for(index=0;index<card_num;index++){
	player->card_num=card_num;
	ncsGrafSetCard(player,index,COLOR_BLUE,"X","",0);
    }

//    players->wnd=newwin(6+2,30+2,24-6-2,(80-30-2)/2);
    if(pos==NCS_GRAF_TABLE_BOTTOM){
	player->size[0]=6+2;
	player->size[1]=30+2;
	player->pos[0]=NCS_GRAF_TABLE_SIZE_Y-6-2;
	player->pos[1]=(NCS_GRAF_TABLE_SIZE_X-30-2)/2;
    }
    if(pos==NCS_GRAF_TABLE_TOP){
	player->size[0]=6+2;
	player->size[1]=30+2;
	player->pos[0]=0;
	player->pos[1]=(NCS_GRAF_TABLE_SIZE_X-30-2)/2;
    }
    if(pos==NCS_GRAF_TABLE_LEFT){
	player->size[0]=8+2;
	player->size[1]=20+2;
	player->pos[0]=(NCS_GRAF_TABLE_SIZE_Y-8-2)/2;
	player->pos[1]=0;
    }
    if(pos==NCS_GRAF_TABLE_RIGHT){
	player->size[0]=8+2;
	player->size[1]=20+2;
	player->pos[0]=(NCS_GRAF_TABLE_SIZE_Y-8-2)/2;
	player->pos[1]=NCS_GRAF_TABLE_SIZE_X-20-2;
    }

    if(pos==NCS_GRAF_TABLE_BOTTOM || pos==NCS_GRAF_TABLE_TOP){
	player->name_pos[0]=1;
	player->name_pos[1]=(30+2-strlen(player->name))/2;
	player->money_pos[0]=2;
	player->money_pos[1]=1;
	player->status_pos[0]=2;
	player->status_pos[1]=30+1-strlen(player->status_text);
	player->cards_pos[0]=3;
	player->cards_pos[1]=1;
    }

    if(pos==NCS_GRAF_TABLE_LEFT || pos==NCS_GRAF_TABLE_RIGHT){
	player->name_pos[0]=1;
	player->name_pos[1]=(22-strlen(player->name))/2;
	player->money_pos[0]=2;
	player->money_pos[1]=1;
	player->status_pos[0]=3;
	player->status_pos[1]=1;
	player->cards_pos[0]=5;
	player->cards_pos[1]=1;
    }

    player->enabled=enabled;

    return;
}

void ncsGrafSetCard(	const struct ncs_graf_player_t* player,\
			int index,\
			int color,\
			const char* val,\
			const char* suit,
			int selected)
{
//тут проверка на правильность ввода данных

    strncpy(player->cards[index].name,val,19);

    player->cards[index].name[19]='\0';
    strncat(player->cards[index].name,\
	    suit,\
	    19-strlen(player->cards[index].name));
    player->cards[index].name[19]='\0';
    player->cards[index].color=color;
    player->cards[index].selected=selected;

    return;
}

void ncsGrafSetBankCard(const struct ncs_graf_bank_t* bank,\
			int index,\
			int color,\
			const char* val,\
			const char* suit,
			int selected)
{
//тут проверка на правильность ввода данных

    strncpy(bank->cards[index].name,val,19);

    bank->cards[index].name[19]='\0';
    strncat(bank->cards[index].name,\
	    suit,\
	    19-strlen(bank->cards[index].name));
    bank->cards[index].name[19]='\0';
    bank->cards[index].color=color;
    bank->cards[index].selected=selected;

    return;
}


void ncsInitColorPairs()
{
    int count=0;
    int index_text_color=0,index_back_color=0;

    for(index_text_color=0;index_text_color<8;index_text_color++){
	for(index_back_color=0;index_back_color<8;index_back_color++){
	    init_pair(count,index_text_color,index_back_color);
	    count++;
	}
    }
    return;
}

void ncsStartGraf(struct ncs_graf_table_t *tbl)
{
    int index_player=0;
    int index_card=0;

    tcgetattr(0,&stored_settings);
    setlocale(LC_ALL, "");

    initscr();
    start_color();
    refresh();

    cbreak();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);

    ncsInitColorPairs();

    CUR_TEXT_COLOR=COLOR_WHITE;
    CUR_BACK_COLOR=COLOR_BLACK;

    tbl->bank.wnd=newwin(tbl->bank.size[0],\
			tbl->bank.size[1],\
			tbl->bank.pos[0],\
			tbl->bank.pos[1]);
    for(index_card=0;\
	index_card<tbl->bank.card_num;\
	index_card++){
	tbl->bank.cards[index_card].wnd=derwin(	tbl->bank.wnd,\
						tbl->card_size[0],\
						tbl->card_size[1],\
						tbl->bank.cards_pos[0],\
						tbl->bank.cards_pos[1]+tbl->card_size[1]*index_card
					      );
    }

    tbl->input.wnd=newwin(tbl->input.size[0],\
			tbl->input.size[1],\
			tbl->input.pos[0],\
			tbl->input.pos[1]);

    tbl->exit_btn.wnd=newwin(tbl->exit_btn.size[0],\
			tbl->exit_btn.size[1],\
			tbl->exit_btn.pos[0],\
			tbl->exit_btn.pos[1]);

    tbl->pass_btn.wnd=newwin(tbl->pass_btn.size[0],\
			tbl->pass_btn.size[1],\
			tbl->pass_btn.pos[0],\
			tbl->pass_btn.pos[1]);

    ncsSetWndColor(stdscr,CUR_TEXT_COLOR,CUR_BACK_COLOR);
    for(index_player=0;index_player<GRAF_MAX_PLAYERS;index_player++){
	if(tbl->players[index_player]!=NULL){
	    tbl->players[index_player]->wnd=newwin(tbl->players[index_player]->size[0],\
					    tbl->players[index_player]->size[1],\
					    tbl->players[index_player]->pos[0],\
					    tbl->players[index_player]->pos[1]);
	    for(index_card=0;\
		index_card<tbl->players[index_player]->card_num;\
		index_card++){
		tbl->players[index_player]->cards[index_card].wnd=derwin(tbl->players[index_player]->wnd,\
					    tbl->card_size[0],\
					    tbl->card_size[1],\
					    tbl->players[index_player]->cards_pos[0],\
					    tbl->players[index_player]->cards_pos[1]+tbl->card_size[1]*index_card
					    );
	    }

//	    tbl->players[index]->wnd=newwin(6+2,30+2,24-6-2,(80-30-2)/2);
//	    tbl->players[index]->wnd=newwin(40,40,1,0);
	}
    }

    clear();
    return;
}

int ncsSetColor(unsigned int TextColor,unsigned int BackColor)
{
    if(TextColor>7 || BackColor>7){
	return -1;
    }
    attrset(COLOR_PAIR(TextColor*8+BackColor));
//    CUR_TEXT_COLOR=TextColor;
//    CUR_BACK_COLOR=BackColor;

    return 0;
}

int ncsSetWndColor(WINDOW *wnd,unsigned int TextColor,unsigned int BackColor)
{
    if(TextColor>7 || BackColor>7){
	printw("BadColor!!!");
	return -1;
    }
    wbkgdset(wnd,COLOR_PAIR(TextColor*8+BackColor));
    CUR_TEXT_COLOR=TextColor;
    CUR_BACK_COLOR=BackColor;

    return 0;
}

void ncsPrintInWnd(WINDOW* wnd,const unsigned int *pos,const char *text)
{
    wmove(wnd,pos[0],pos[1]);
    wprintw(wnd,"%s",text);

    return;
}

void ncsShowBank(const struct ncs_graf_table_t* tbl)
{
    int index_card=0;
    int indexA=0,indexB=0;

    ncsSetWndColor(tbl->bank.wnd,COLOR_WHITE,COLOR_GREEN);
    wclear(tbl->bank.wnd);
    ncsPrintInWnd(tbl->bank.wnd,\
		tbl->bank.money_pos,\
		tbl->bank.money_text);
    ncsPrintInWnd(tbl->bank.wnd,\
		tbl->bank.timer_pos,\
		tbl->bank.timer_text);
    box(tbl->bank.wnd,0,0);
    
//    wrefresh(tbl->bank.wnd);
    for(index_card=0;index_card<tbl->bank.card_num;index_card++){
//	wrefresh(tbl->bank.cards[index_card].wnd);
	ncsSetWndColor(tbl->bank.cards[index_card].wnd,\
		    tbl->bank.cards[index_card].color,\
		    COLOR_WHITE);
	wclear(tbl->bank.cards[index_card].wnd);
	box(tbl->bank.cards[index_card].wnd,0,0);
	wmove(tbl->bank.cards[index_card].wnd,1,1);

	if(tbl->bank.cards[index_card].name[1]!='\0'){
	    wprintw(tbl->bank.cards[index_card].wnd,\
		    "%s",\
		    tbl->bank.cards[index_card].name);
	}
	else{
	    for(indexA=1;indexA<(tbl->card_size[1]-1);indexA++){
		for(indexB=1;indexB<(tbl->card_size[0]-1);indexB++){
		    wmove(	tbl->bank.cards[index_card].wnd,\
				indexB,indexA);
		    wprintw(tbl->bank.cards[index_card].wnd,\
			    "%s",\
			    tbl->bank.cards[index_card].name);
		}
	    }
	}
	//wrefresh(tbl->bank.cards[index_card].wnd);
    }
    refresh();
}

void ncsShowInput(const struct ncs_graf_table_t* tbl)
{
    ncsSetWndColor(tbl->input.wnd,COLOR_GREEN,COLOR_BLACK);
    wclear(tbl->input.wnd);
    ncsPrintInWnd(tbl->input.wnd,\
		tbl->input.title_pos,\
		tbl->input.title);
    ncsPrintInWnd(tbl->input.wnd,\
		tbl->input.buffer_pos,\
		tbl->input.buffer);
    if(tbl->input.selected){
        box(tbl->input.wnd,0,0);
    }
    
    wrefresh(tbl->input.wnd);
//    refresh();
}

void ncsShowBtn(const struct ncs_graf_button_t* btn)
{
    ncsSetWndColor(btn->wnd,COLOR_WHITE,COLOR_BLUE);
    wclear(btn->wnd);
    ncsPrintInWnd(btn->wnd,\
		btn->title_pos,\
		btn->title);
    if(btn->selected){
        box(btn->wnd,0,0);
    }
    
    wrefresh(btn->wnd);
//    refresh();
}


void ncsShow(const struct ncs_graf_table_t *tbl)
{
    int index_player=0;
    int index_card=0;

    int indexA=0,indexB=0;

//    SetColor(COLOR_WHITE,COLOR_BLACK);
//    SetWndColor(stdscr,COLOR_WHITE,CUR_BACK_COLOR);
//    SetWndColor(stdscr,COLOR_WHITE,COLOR_GREEN);

    move(0,0);
    clear();

//    refresh();

    for(index_player=0;index_player<GRAF_MAX_PLAYERS;index_player++){
	if(tbl->players[index_player]!=NULL){
	    ncsSetWndColor(tbl->players[index_player]->wnd,COLOR_WHITE,COLOR_GREEN);
//	    wbkgdset(tbl->players[index]->wnd,COLOR_PAIR(COLOR_WHITE*8+COLOR_GREEN));

	    wclear(tbl->players[index_player]->wnd);
	    ncsPrintInWnd(tbl->players[index_player]->wnd,\
			tbl->players[index_player]->name_pos,\
			tbl->players[index_player]->name);
	    ncsPrintInWnd(tbl->players[index_player]->wnd,\
			tbl->players[index_player]->money_pos,\
			tbl->players[index_player]->money_text);
	    ncsPrintInWnd(tbl->players[index_player]->wnd,\
			tbl->players[index_player]->status_pos,\
			tbl->players[index_player]->status_text);
	    box(tbl->players[index_player]->wnd,0,0);
	    
//	    wrefresh(tbl->players[index_player]->wnd);
	    for(index_card=0;index_card<tbl->players[index_player]->card_num;index_card++){
//		wrefresh(tbl->players[index_player]->cards[index_card].wnd);
		ncsSetWndColor(tbl->players[index_player]->cards[index_card].wnd,\
			    tbl->players[index_player]->cards[index_card].color,\
			    COLOR_WHITE);
		wclear(tbl->players[index_player]->cards[index_card].wnd);
		box(tbl->players[index_player]->cards[index_card].wnd,0,0);
		wmove(tbl->players[index_player]->cards[index_card].wnd,1,1);
//		SetColor(tbl->players[index_player]->cards[index_card].color,CUR_BACK_COLOR);
		if(tbl->players[index_player]->cards[index_card].name[1]!='\0'){
		    wprintw(tbl->players[index_player]->cards[index_card].wnd,\
			    "%s",\
			    tbl->players[index_player]->cards[index_card].name);
		}
		else{
		    for(indexA=1;indexA<(tbl->card_size[1]-1);indexA++){
			for(indexB=1;indexB<(tbl->card_size[0]-1);indexB++){
			    wmove(	tbl->players[index_player]->cards[index_card].wnd,\
					indexB,indexA);
			    wprintw(tbl->players[index_player]->cards[index_card].wnd,\
				    "%s",\
				    tbl->players[index_player]->cards[index_card].name);
			}
		    }
		}
//		wrefresh(tbl->players[index_player]->cards[index_card].wnd);
	    }
//	    wrefresh(tbl->players[index_player]->wnd);
	}
    }
    ncsShowBank(tbl);
    if(tbl->input.enabled){
        ncsShowInput(tbl);
    }

    if(tbl->exit_btn.enabled){
        ncsShowBtn(&tbl->exit_btn);
    }

    if(tbl->pass_btn.enabled){
        ncsShowBtn(&tbl->pass_btn);
    }

    refresh();
//    getch();

    return;
}

void ncsEndGraf()
{
    endwin();
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

//---------------------------Thread funcs
void* ncsGrafFunc(void* data)
{
    

    return NULL;    
}

void ncsChElem(int _step)
{
    int ok=0;
    int step=1;

    if(_step<0){
	step=-1;
    }
    
    while(ok==0){
	selected_index+=step/abs(step);
	if(selected_index<0){
    	    selected_index+=3;
	}
	if(selected_index>=3){
    	    selected_index-=3;
	}
	main_tbl->exit_btn.selected=0;
	main_tbl->input.selected=0;
	main_tbl->pass_btn.selected=0;

	switch(selected_index){
	    case 0: if(main_tbl->exit_btn.enabled){
			ok=1;
			main_tbl->exit_btn.selected=1;
		    }
		    break;
	    case 1: if(main_tbl->input.enabled){
			ok=1;
			main_tbl->input.selected=1;
		    }
		    break;
	    case 2: if(main_tbl->pass_btn.enabled){
			ok=1;
			main_tbl->pass_btn.selected=1;
		    }
		    break;
	};
	if(main_tbl->input.enabled){
	    wrefresh(main_tbl->input.wnd);
	}
	wrefresh(main_tbl->exit_btn.wnd);
	wrefresh(main_tbl->pass_btn.wnd);
	refresh();
    }
}

void* ncsControlsFunc(void* data)
{
    int c=0;
    char s[3];
    int len=0;
    int res=0;
    int index=0;

    while(1){
	c=getch();
	if(c==KEY_LEFT || c==KEY_UP){
	    ncsChElem(-1);
	    ncsShowBtn(&main_tbl->exit_btn);
	    ncsShowBtn(&main_tbl->pass_btn);
	    if(main_tbl->input.enabled){
	        ncsShowInput(main_tbl);
	    }
	}
	if(c==KEY_RIGHT || c==KEY_DOWN || c=='\t'){
	    ncsChElem(1);
	    ncsShowBtn(&main_tbl->exit_btn);
	    ncsShowBtn(&main_tbl->pass_btn);
	    if(main_tbl->input.enabled){
	        ncsShowInput(main_tbl);
	    }
	}
	refresh();
	if(c==27){
	    graf_exit_event();
	}
	if(c==10){
	    if(	main_tbl->input.enabled==1 && \
		main_tbl->input.selected==1){
		len=strlen(main_tbl->input.buffer);
		res=0;
		for(index=0;index<len;index++){
		    res=res*10+main_tbl->input.buffer[index]-'0';	
		}
		graf_bet_event(res);
	    }
	    if(	main_tbl->exit_btn.enabled==1 &&\
		main_tbl->exit_btn.selected==1){
		graf_exit_event();
	    }
	    if(	main_tbl->pass_btn.enabled==1 &&\
		main_tbl->pass_btn.selected==1){
		graf_pass_event();
	    }
	}
	if(	main_tbl->input.enabled==1 && \
		main_tbl->input.selected==1 &&\
		 isdigit(c)){
	    len=strlen(main_tbl->input.buffer);
	    main_tbl->input.buffer[len]=c;
	    main_tbl->input.buffer[len+1]='\0';
	    ncsShowInput(main_tbl);
	}
	refresh();
    }

    return NULL;
}

void ncsTempExit()
{
    ncsEndGraf();
    exit(0);
}

void ncsTempBet(int sum)
{
    grafHideInput();
}

void ncsTempPass()
{
    return;
}