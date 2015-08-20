#include "graf_list.h"

static struct termios stored_settings;

static unsigned int CUR_TEXT_COLOR=COLOR_BLACK;
static unsigned int CUR_BACK_COLOR=COLOR_WHITE;

static struct ncs_graf_list_t main_list;

static int btn_index=0;

void (*graf_list_exit_event)(void)=ncsTempListExit;
void (*graf_list_select_event)(int id)=ncsTempListSelect;
void (*graf_list_create_event)(void)=ncsTempListCreate;
void (*graf_list_refresh_event)(void)=ncsTempListRefresh;

//-------------------------------------API connect block

//------------------------------------API block
void grafInitList()
{
    int index=0;
    pthread_t thread;

    pthread_create(&thread,NULL,ncsListControlsFunc,NULL);
    pthread_detach(thread);
    ncsListStartGraf(&main_list);
}

void grafDrawTableList(graf_list_t* list)
{
    int index=0;

    main_list=(*list);

    ncsListShow(&main_list);
    wrefresh(main_list.exit_btn.wnd);
    wrefresh(main_list.create_btn.wnd);
    wrefresh(main_list.refresh_btn.wnd);

    wrefresh(main_list.wnd);
    refresh();
//    getch();
}
void grafDrawMsgList(const char* msg)
{
    int pos[2]={0,0};
    clear();
    ncsPrintInWnd(stdscr,pos,msg);
}

void grafExitList()
{
    ncsListEndGraf();
}

//---------------------------------Ncurses opereations block
void ncsListGrafInit(	struct ncs_graf_list_t* list,\
			const struct graf_list_t* api_list)
{
    int index=0;

    strncpy(list->title,api_list->title,256-1);
    list->title[256-1]='\0';
    for(index=0;index<MAX_TABLES_COUNT;index++){
	list->tables[index]=api_list->tables[index];
    }
    list->selected_index=0;

    list->size[0]=22+2;
    list->size[1]=20+2;
    list->pos[0]=(NCS_GRAF_TABLE_SIZE_Y-22-2)/2;
    list->pos[1]=(NCS_GRAF_TABLE_SIZE_X-20-2)/2;
    list->enabled=1;
    list->selected=1;

    list->refresh_btn.enabled=1;
    list->refresh_btn.selected=0;
    strcpy(list->refresh_btn.title,"REFRESH");

    list->create_btn.size[0]=1+2;
    list->create_btn.size[1]=7+2;
    list->create_btn.pos[0]=NCS_GRAF_TABLE_SIZE_Y-7-2;
    list->create_btn.pos[1]=NCS_GRAF_TABLE_SIZE_X-7-2;
    list->create_btn.title_pos[0]=1;
    list->create_btn.title_pos[1]=1;

    list->create_btn.enabled=1;
    list->create_btn.selected=0;
    strcpy(list->create_btn.title,"CREATE ");

    list->create_btn.size[0]=1+2;
    list->create_btn.size[1]=7+2;
    list->create_btn.pos[0]=NCS_GRAF_TABLE_SIZE_Y-4-2;
    list->create_btn.pos[1]=NCS_GRAF_TABLE_SIZE_X-7-2;
    list->create_btn.title_pos[0]=1;
    list->create_btn.title_pos[1]=1;

    list->exit_btn.enabled=1;
    list->exit_btn.selected=0;
    strcpy(list->exit_btn.title,"EXIT   ");

    list->exit_btn.size[0]=1+2;
    list->exit_btn.size[1]=7+2;
    list->exit_btn.pos[0]=NCS_GRAF_TABLE_SIZE_Y-1-2;
    list->exit_btn.pos[1]=NCS_GRAF_TABLE_SIZE_X-7-2;
    list->exit_btn.title_pos[0]=1;
    list->exit_btn.title_pos[1]=1;

    return;
}

void ncsListStartGraf(struct ncs_graf_list_t *list)
{
    tcgetattr(0,&stored_settings);
    setlocale(LC_ALL, "");

    initscr();
    start_color();

    cbreak();
    noecho();
    curs_set(0);

    keypad(stdscr, TRUE);

    ncsInitColorPairs();

    CUR_TEXT_COLOR=COLOR_WHITE;
    CUR_BACK_COLOR=COLOR_BLACK;

    ncsSetWndColor(stdscr,CUR_TEXT_COLOR,CUR_BACK_COLOR);
    clear();

    CUR_TEXT_COLOR=COLOR_WHITE;
    CUR_BACK_COLOR=COLOR_BLACK;

    list->wnd=newwin(	list->size[0],\
			list->size[1],\
			list->pos[0],\
			list->pos[1]);

    ncsSetWndColor(list->exit_btn.wnd,COLOR_WHITE,COLOR_BLUE);
    list->exit_btn.wnd=newwin(list->exit_btn.size[0],\
			list->exit_btn.size[1],\
			list->exit_btn.pos[0],\
			list->exit_btn.pos[1]);
    wclear(list->exit_btn.wnd);
    wrefresh(list->exit_btn.wnd);

    list->create_btn.wnd=newwin(list->create_btn.size[0],\
			list->create_btn.size[1],\
			list->create_btn.pos[0],\
			list->create_btn.pos[1]);
    wclear(list->create_btn.wnd);
    wrefresh(list->create_btn.wnd);

    list->refresh_btn.wnd=newwin(list->refresh_btn.size[0],\
			list->refresh_btn.size[1],\
			list->refresh_btn.pos[0],\
			list->refresh_btn.pos[1]);
    wclear(list->refresh_btn.wnd);
    wrefresh(list->refresh_btn.wnd);

    return;
}

void ncsListShow(const struct ncs_graf_list_t *list)
{
    int indexA=0,indexB=0;

    move(0,0);
    clear();

    ncsSetWndColor(list->wnd,COLOR_BLACK,COLOR_WHITE);	
    wmove(list->wnd,1,1);
    wprintw(list->wnd,list->title);
    for(indexA=0;indexA<MAX_TABLES_COUNT;indexA++){
	ncsSetWndColor(list->wnd,COLOR_WHITE,COLOR_BLACK);	
	if(list->selected_index==indexA){
	    ncsSetWndColor(list->wnd,COLOR_BLUE,COLOR_WHITE);	
	}
	if(list->tables[indexA].enabled==0){
	    continue;
	}
	wmove(list->wnd,indexA+2,1);
	wprintw(list->wnd,"%d - %d players",\
		list->tables[indexA].id,\
		list->tables[indexA].players_count);
    }
    ncsSetWndColor(list->wnd,COLOR_WHITE,COLOR_WHITE);	
    box(list->wnd,0,0);
    ncsSetWndColor(list->wnd,COLOR_BLACK,COLOR_WHITE);	
    if(list->selected==1){
        box(list->wnd,0,0);
    }
    wrefresh(list->wnd);

    if(list->exit_btn.enabled){
        ncsShowBtn(&list->exit_btn);
    }

    if(list->create_btn.enabled){
        ncsShowBtn(&list->create_btn);
    }

    if(list->refresh_btn.enabled){
        ncsShowBtn(&list->refresh_btn);
    }

    refresh();
//    getch();

    return;
}

void ncsListEndGraf()
{
    endwin();
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

//---------------------------Thread funcs
void ncsListChElem(int _step)
{
    int ok=0;
    int step=1;

    if(_step<0){
	step=-1;
    }
    
    while(ok==0){
	btn_index+=step/abs(step);
	if(btn_index<0){
    	    btn_index+=4;
	}
	if(btn_index>=4){
    	    btn_index-=4;
	}
	main_list.exit_btn.selected=0;
	main_list.refresh_btn.selected=0;
	main_list.create_btn.selected=0;
	main_list.selected=0;

	switch(btn_index){
	    case 0: if(main_list.refresh_btn.enabled){
			ok=1;
			main_list.refresh_btn.selected=1;
		    }
		    break;
	    case 1: if(main_list.create_btn.enabled){
			ok=1;
			main_list.create_btn.selected=1;
		    }
		    break;
	    case 2: if(main_list.exit_btn.enabled){
			ok=1;
			main_list.exit_btn.selected=1;
		    }
		    break;
	    case 4: if(main_list.enabled){
			ok=1;
			main_list.selected=1;
		    }
		    break;
	};

	wrefresh(main_list.exit_btn.wnd);
	wrefresh(main_list.create_btn.wnd);
	wrefresh(main_list.refresh_btn.wnd);
	wrefresh(main_list.wnd);
	refresh();
    }
}

void* ncsListControlsFunc(void* data)
{
    int c=0;
    char s[3];
    int len=0;
    int res=0;
    int index=0;

    while(1){
	c=getch();
	if(c==KEY_UP && main_list.selected==1){
	    main_list.selected_index--;
	    if(main_list.selected_index<0){
		main_list.selected_index=0;
	    }
	}
	if(c==KEY_DOWN && main_list.selected==1){
	    main_list.selected_index++;
	    if(main_list.selected_index>=MAX_TABLES_COUNT){
		main_list.selected_index=MAX_TABLES_COUNT-1;
	    }
	}
	if(c==KEY_LEFT){
	    ncsListChElem(-1);
	    ncsShowBtn(&main_list.exit_btn);
	    ncsShowBtn(&main_list.create_btn);
	    ncsShowBtn(&main_list.refresh_btn);
	}
	if(c==KEY_RIGHT || c=='\t'){
	    ncsListChElem(1);
	    ncsShowBtn(&main_list.exit_btn);
	    ncsShowBtn(&main_list.create_btn);
	    ncsShowBtn(&main_list.refresh_btn);
	}

//	refresh();
	if(c==27){
	    graf_exit_event();
	    return NULL;
	}
	if(c==10){
	    if(	main_list.enabled==1 && \
		main_list.selected==1){
		graf_list_select_event(main_list.selected_index);
	    }
	    if(	main_list.exit_btn.enabled==1 &&\
		main_list.exit_btn.selected==1){
		graf_list_exit_event();
		return NULL;
	    }
	    if(	main_list.refresh_btn.enabled==1 &&\
		main_list.refresh_btn.selected==1){
		graf_list_refresh_event();
	    }
	    if(	main_list.create_btn.enabled==1 &&\
		main_list.create_btn.selected==1){
		graf_list_create_event();
	    }
	}
	refresh();
    }

    return NULL;
}

void ncsTempListExit()
{
    ncsListEndGraf();
    exit(0);
}

void ncsTempListSelect(int id)
{
//    grafHideInput();
}

void ncsTempListCreate()
{
    return;
}

void ncsTempListRefresh()
{
    return;
}