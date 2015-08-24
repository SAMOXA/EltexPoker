#include "graf_static_game.h"

static struct termios stored_settings;
static int initiated=0;

static struct ncs_graf_table_t main_tbl;

static unsigned int CUR_TEXT_COLOR=COLOR_WHITE;
static unsigned int CUR_BACK_COLOR=COLOR_BLACK;

void (*graf_exit_event)(void);
void (*graf_bet_event)(int sum);
void (*graf_pass_event)(void);

int ncsSetColor(WINDOW *wnd,unsigned int TextColor,unsigned int BackColor)
{
    if(TextColor>7 || BackColor>7){
		printw("BadColor!!!");
		return -1;
    }
    wbkgdset(wnd,COLOR_PAIR(TextColor*8+BackColor));

    return 0;
}

int ncsSetWndColor(WINDOW *wnd,unsigned int TextColor,unsigned int BackColor)
{
    if(ncsSetColor(wnd,TextColor,BackColor)<0){
        return -1;
    }

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

void ncsResize(int signo)
{
	struct winsize size;
	int pos=0;
	int index_player=0,index_card=0;
	int max_y,max_x;
	struct ncs_graf_player_t *player=NULL;
	struct ncs_graf_bank_t *bank=NULL;
	struct ncs_graf_card_t *card=NULL;
	struct ncs_graf_input_t *input=NULL;
	struct ncs_graf_button_t *btn=NULL;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
	max_y=size.ws_row;
	max_x=size.ws_col;

	ncsSetWndColor(stdscr,main_tbl.text_color,main_tbl.back_color);
	wclear(stdscr);

	//PLAYERS INIT
	for(index_player=0;index_player<GRAF_MAX_PLAYERS;index_player++){
		pos=index_player;
		player=&(main_tbl.players[index_player]);
		if(pos==NCS_GRAF_TABLE_BOTTOM){
			player->size[0]=main_tbl.top_player_size[0]*max_y;
			player->size[1]=main_tbl.top_player_size[1]*max_x;
			player->pos[0]=max_y-player->size[0];
			player->pos[1]=(max_x-player->size[1])/2;
    	}
    	if(pos==NCS_GRAF_TABLE_TOP){
			player->size[0]=main_tbl.top_player_size[0]*max_y;
			player->size[1]=main_tbl.top_player_size[1]*max_x;
			player->pos[0]=0;
			player->pos[1]=(max_x-player->size[1])/2;
    	}
    	if(pos==NCS_GRAF_TABLE_LEFT){
			player->size[0]=main_tbl.side_player_size[0]*max_y;
			player->size[1]=main_tbl.side_player_size[1]*max_x;
			player->pos[0]=(max_y-player->size[0])/2;
			player->pos[1]=0;
    	}
		if(pos==NCS_GRAF_TABLE_RIGHT){
			player->size[0]=main_tbl.side_player_size[0]*max_y;
			player->size[1]=main_tbl.side_player_size[1]*max_x;
			player->pos[0]=(max_y-player->size[0])/2;
			player->pos[1]=max_x-player->size[1];
    	}
    
		if(pos==NCS_GRAF_TABLE_BOTTOM || pos==NCS_GRAF_TABLE_TOP){
			player->name_pos[0]=1;
			player->name_pos[1]=1;//(player->size[1]-strlen(player->name))/2;
			player->money_pos[0]=2;
			player->money_pos[1]=1;
			player->status_pos[0]=2;
			player->status_pos[1]=player->size[1]/2;//player->size[1]-1-strlen(player->status_text);
			player->cards_pos[0]=3;
			player->cards_pos[1]=1;
    	}

    	if(pos==NCS_GRAF_TABLE_LEFT || pos==NCS_GRAF_TABLE_RIGHT){
			player->name_pos[0]=1;
			player->name_pos[1]=1;//(player->size[1]-strlen(player->name))/2;
			player->money_pos[0]=2;
			player->money_pos[1]=1;
			player->status_pos[0]=3;
			player->status_pos[1]=1;
			player->cards_pos[0]=5;
			player->cards_pos[1]=1;
    	}
//		if(player->wnd!=NULL){
		if(initiated){
			delwin(player->wnd);
		}
		player->last_enabled=0;
		player->wnd=newwin(player->size[0],\
					    player->size[1],\
					    player->pos[0],\
					    player->pos[1]);
		ncsSetWndColor(	player->wnd,\
						main_tbl.player_text_color,\
						main_tbl.player_back_color);
		wclear(player->wnd);
		for(index_card=0;index_card<GRAF_CARDS_PER_PLAYER_COUNT;index_card++){
			card=&(player->cards[index_card]);
			card->pos[0]=player->cards_pos[0];
			card->pos[1]=player->cards_pos[1]+main_tbl.card_size[1]*index_card;
//			if(card->wnd!=NULL){
			if(initiated){
				delwin(card->wnd);
			}
			card->wnd=derwin(player->wnd,
								main_tbl.card_size[0],\
		    					main_tbl.card_size[1],\
		    					card->pos[0],\
		    					card->pos[1]);
			ncsSetWndColor(	card->wnd,\
							main_tbl.card_text_color,\
							main_tbl.card_back_color);
			wclear(card->wnd);
		}
	}
	
	//BANK & MSG INIT
	bank=&(main_tbl.bank);

	bank->size[0]=main_tbl.side_player_size[0]*max_y;
	bank->size[1]=main_tbl.top_player_size[1]*max_x;
	bank->pos[0]=(max_y-bank->size[0])/2;
	bank->pos[1]=(max_x-bank->size[1])/2;
	bank->money_pos[0]=1;
	bank->money_pos[1]=1;
	bank->timer_pos[0]=1;
	bank->timer_pos[1]=bank->size[1]/2;//bank->size[1]-1-strlen(tbl->bank.timer_text);
	bank->cards_pos[0]=2;
	bank->cards_pos[1]=1;

//	if(main_tbl.msg_wnd!=NULL){
	if(initiated){
		delwin(main_tbl.msg_wnd);
	}
	main_tbl.msg_wnd=newwin(1,bank->size[1],bank->pos[0],bank->pos[1]);

//	if(bank->wnd!=NULL){
	if(initiated){
		delwin(bank->wnd);
	}
	bank->wnd=newwin(bank->size[0]-1,bank->size[1],bank->pos[0]+1,bank->pos[1]);
	ncsSetWndColor(bank->wnd,main_tbl.bank_text_color,main_tbl.bank_back_color);
	wclear(bank->wnd);
	wrefresh(bank->wnd);
	refresh();

	for(index_card=0;index_card<GRAF_CARDS_ON_TABLE_COUNT;index_card++){
		card=&(bank->cards[index_card]);
		card->pos[0]=bank->cards_pos[0];
		card->pos[1]=bank->cards_pos[1]+main_tbl.card_size[1]*index_card;
//		if(card->wnd!=NULL){
		if(initiated){
			delwin(card->wnd);
		}
		card->wnd=derwin(bank->wnd,main_tbl.card_size[0],main_tbl.card_size[1],card->pos[0],card->pos[1]);
		ncsSetWndColor(	card->wnd,main_tbl.card_text_color,main_tbl.card_back_color);
		wclear(card->wnd);
	}

	//INPUT
	input=&(main_tbl.input);
	input->size[0]=main_tbl.top_player_size[0]*max_y;
	input->size[1]=main_tbl.side_player_size[1]*max_x;
	input->pos[0]=max_y-input->size[0];
	input->pos[1]=0;
	input->title_pos[0]=1;
	input->title_pos[1]=1;
	input->buffer_pos[0]=2;
	input->buffer_pos[1]=1;
//	if(input->wnd!=NULL){
	if(initiated){
		delwin(input->wnd);
	}
	input->wnd=newwin(input->size[0],input->size[1],input->pos[0],input->pos[1]);
	ncsSetWndColor(input->wnd,main_tbl.text_color,main_tbl.back_color);
	wclear(input->wnd);

	//BUTTONS
	btn=&(main_tbl.exit_btn);
	btn->size[0]=3;
	btn->size[1]=4+2;
	btn->pos[0]=max_y-btn->size[0];
	btn->pos[1]=max_x-btn->size[1];
	btn->title_pos[0]=1;
	btn->title_pos[1]=1;
	strcpy(btn->title,"EXIT");
//	if(btn->wnd!=NULL){
	if(initiated){
		delwin(btn->wnd);
	}
	btn->wnd=newwin(btn->size[0],btn->size[1],btn->pos[0],btn->pos[1]);
	ncsSetWndColor(btn->wnd,main_tbl.btn_text_color,main_tbl.btn_back_color);
	wclear(btn->wnd);

	btn=&(main_tbl.pass_btn);
	btn->size[0]=3;
	btn->size[1]=4+2;
	btn->pos[0]=max_y-btn->size[0]*2;
	btn->pos[1]=max_x-btn->size[1];
	btn->title_pos[0]=1;
	btn->title_pos[1]=1;
	strcpy(btn->title,"PASS");
//	if(btn->wnd!=NULL){
	if(initiated){
		delwin(btn->wnd);
	}
	btn->wnd=newwin(btn->size[0],btn->size[1],btn->pos[0],btn->pos[1]);
	ncsSetWndColor(btn->wnd,main_tbl.btn_text_color,main_tbl.btn_back_color);
	wclear(btn->wnd);

	ncsShow();
}

void ncsInitMainTable(struct graf_table_t *tbl)
{
	memset(&main_tbl,0,sizeof(struct ncs_graf_table_t));

	main_tbl.top_player_size[0]=0.3;
	main_tbl.top_player_size[1]=0.5;

	main_tbl.side_player_size[0]=0.4;
	main_tbl.side_player_size[1]=0.3;

	main_tbl.bank_size[0]=0.4;
	main_tbl.bank_size[1]=0.4;

	main_tbl.text_color=COLOR_WHITE;
	main_tbl.back_color=COLOR_BLACK;

	main_tbl.player_text_color=COLOR_WHITE;
	main_tbl.player_back_color=COLOR_GREEN;

	main_tbl.sel_player_text_color=COLOR_WHITE;
	main_tbl.sel_player_back_color=COLOR_BLUE;

	main_tbl.bank_text_color=COLOR_WHITE;
	main_tbl.bank_back_color=COLOR_GREEN;

	main_tbl.btn_text_color=COLOR_WHITE;
	main_tbl.btn_back_color=COLOR_BLUE;

	main_tbl.card_text_color=COLOR_BLUE;
	main_tbl.card_back_color=COLOR_WHITE;

	main_tbl.sel_card_text_color=COLOR_BLUE;
	main_tbl.sel_card_back_color=COLOR_YELLOW;

	signal(SIGWINCH, ncsResize);

	ncsResize(SIGWINCH);//создает(пересоздает) окна, задает цвета для них

	return;
}

void ncsInitGame(struct graf_table_t *tbl)//инициализация
{
	tcgetattr(0,&stored_settings);
  	setlocale(LC_ALL, "");

  	initscr();
//  	pthread_mutex_init(&graf_lock,NULL);
  	start_color();

  	cbreak();
  	noecho();
  	curs_set(0);
  	keypad(stdscr, TRUE);

  	ncsInitColorPairs();

	ncsInitMainTable(tbl);

  	initiated=1;
 	ncsSetWndColor(stdscr,main_tbl.text_color,main_tbl.back_color);
	
	clear();
	refresh();
  	return;
}

void ncsSetGame(struct graf_table_t *tbl)
{
	int index_player=0,index_card=0;
	struct ncs_graf_player_t *player=NULL;
	struct ncs_graf_bank_t *bank=NULL;
	struct ncs_graf_card_t *card=NULL;
	struct graf_player_t *api_player=NULL;
	struct graf_bank_t *api_bank=NULL;
	struct graf_card_t *api_card=NULL;
	char suit[5]="";
	int color=0;

	for(index_player=0;index_player<GRAF_MAX_PLAYERS;index_player++){
		player=&(main_tbl.players[index_player]);
		api_player=&(tbl->players[index_player]);

		player->enabled=api_player->enabled;
		player->selected=api_player->selected;

    	strncpy(player->name,api_player->name,GRAF_MAX_NAME_SIZE-1);
    	player->name[GRAF_MAX_NAME_SIZE-1]='\0';

    	strncpy(player->status_text,api_player->status_text,GRAF_MAX_STATUS_TEXT_SIZE-1);
    	player->status_text[GRAF_MAX_STATUS_TEXT_SIZE-1]='\0';

    	strncpy(player->money_text,api_player->money_text,GRAF_MAX_MONEY_TEXT_SIZE-1);
    	player->money_text[GRAF_MAX_MONEY_TEXT_SIZE-1]='\0';

		for(index_card=0;index_card<GRAF_CARDS_PER_PLAYER_COUNT;index_card++){
			card=&(player->cards[index_card]);
			api_card=&(api_player->cards[index_card]);

			strncpy(card->name,api_card->val,3);
    		card->name[19]='\0';
			
			switch(api_card->index_suit){
				case GRAF_INDEX_SPADES: strcpy(suit,NCS_GRAF_SPADES); color=COLOR_BLACK; break;
				case GRAF_INDEX_CLUBS: strcpy(suit,NCS_GRAF_CLUBS); color=COLOR_BLACK; break;
				case GRAF_INDEX_HEARTS: strcpy(suit,NCS_GRAF_HEARTS); color=COLOR_RED; break;
				case GRAF_INDEX_DIAMONDS: strcpy(suit,NCS_GRAF_DIAMONDS); color=COLOR_RED; break;
			};
    		strncat(card->name,suit,4);
		    card->color=color;
		    card->selected=api_card->selected;
		}
	}

	bank=&(main_tbl.bank);
	api_bank=&(tbl->bank);

    strncpy(bank->timer_text,api_bank->timer_text,GRAF_MAX_TIMER_TEXT_SIZE-1);
    bank->timer_text[GRAF_MAX_TIMER_TEXT_SIZE-1]='\0';

    strncpy(bank->money_text,api_bank->money_text,GRAF_MAX_MONEY_TEXT_SIZE-1);
    bank->money_text[GRAF_MAX_MONEY_TEXT_SIZE-1]='\0';

	for(index_card=0;index_card<GRAF_CARDS_ON_TABLE_COUNT;index_card++){
		card=&(bank->cards[index_card]);
		api_card=&(api_bank->cards[index_card]);

		strncpy(card->name,api_card->val,3);
   		card->name[19]='\0';
			
		switch(api_card->index_suit){
			case GRAF_INDEX_SPADES: strcpy(suit,NCS_GRAF_SPADES); color=COLOR_BLACK; break;
			case GRAF_INDEX_CLUBS: strcpy(suit,NCS_GRAF_CLUBS); color=COLOR_BLACK; break;
			case GRAF_INDEX_HEARTS: strcpy(suit,NCS_GRAF_HEARTS); color=COLOR_RED; break;
			case GRAF_INDEX_DIAMONDS: strcpy(suit,NCS_GRAF_DIAMONDS); color=COLOR_RED; break;
		};
   		strncat(card->name,suit,4);
	    card->color=color;
	    card->selected=api_card->selected;
	}

	return;
}

void ncsClearPrint(WINDOW* wnd,unsigned int pos[2],char *text, int len)
{
	char str[128]="";
	char spaces[128]={' '};
	
	strncpy(str,text,len);
	strncat(str,spaces,len-strlen(str));//забиваем остаток строки пробелами - вместо очистки экрана
	ncsPrintInWnd(wnd,pos,str);

	return;
}

void ncsShow()
{
	int size=0;
	unsigned int card_pos[2]={1};
	int index_player=0,index_card=0;
	struct ncs_graf_player_t *player=NULL;
	struct ncs_graf_bank_t *bank=NULL;
	struct ncs_graf_card_t *card=NULL;

	for(index_player=0;index_player<GRAF_MAX_PLAYERS;index_player++){
		player=&(main_tbl.players[index_player]);
		if(player->enabled==0){
			if(player->last_enabled==1){
				wclear(player->wnd);
				wrefresh(player->wnd);
			}
			continue;
		}
		size=player->size[1]-2;
		ncsClearPrint(player->wnd,player->name_pos,player->name,size);
		if(index_player%2==0){
			size=(player->size[1]-2)/2;
		}
		else{
			size=(player->size[1]-2);
		}
		ncsClearPrint(player->wnd,player->money_pos,player->money_text,size);
		ncsClearPrint(player->wnd,player->status_pos,player->status_text,size);

		for(index_card=0;index_card<GRAF_CARDS_PER_PLAYER_COUNT;index_card++){
			card=&(player->cards[index_card]);
			if(card->enabled==0){
				if(card->last_enabled==1){
					wclear(card->wnd);
					box(card->wnd,0,0);
					wrefresh(card->wnd);
				}
				continue;
			}
//тут задать нужный цвет для карты и/или нарисовать рубашку
			ncsPrintInWnd(card->wnd,card_pos,card->name);
			wrefresh(card->wnd);
		}
	wrefresh(player->wnd);
	}

	bank=&(main_tbl.bank);
	ncsClearPrint(bank->wnd,bank->money_pos,bank->money_text,size);
	ncsClearPrint(bank->wnd,bank->timer_pos,bank->timer_text,size);

	for(index_card=0;index_card<GRAF_CARDS_PER_PLAYER_COUNT;index_card++){
		card=&(bank->cards[index_card]);
		if(card->enabled==0){
			if(card->last_enabled==1){
				wclear(card->wnd);
				box(card->wnd,0,0);
				wrefresh(card->wnd);
			}
			continue;
		}
//тут задать нужный цвет для карты и/или нарисовать рубашку
		ncsPrintInWnd(card->wnd,card_pos,card->name);
		wrefresh(card->wnd);
	}

	wrefresh(bank->wnd);

	refresh();	
}

void grafDrawAll(struct graf_table_t *tbl)
{
	if(!initiated){
		ncsInitGame(tbl);
	}

	ncsSetGame(tbl);

	ncsShow();
}

void grafDrawUserMsg(const char* msg){}
void grafExit()
{
    if(initiated){
		initiated=0; 
		endwin();
    }
    tcsetattr(0,TCSANOW,&stored_settings);
	return;
}

void grafShowInput(	struct graf_table_t *tbl,\
			const char* title,\
			const char* default_text){}
void grafHideInput(struct graf_table_t *tbl){}
