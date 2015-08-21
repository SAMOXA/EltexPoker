#include "gtk_header.h"

struct playersGameTableBox *gameTable = NULL; 
void (*graf_exit_event)(void) = funcExit;
void (*graf_bet_event)(int sum) = funcBet;
void (*graf_pass_event)(void) = funcPass;

void funcExit() {
		return;
}

void funcBet(int sum) {
		const char *entry_text;
		
		/* Извлечение текста из окна виджета */
		entry_text = gtk_entry_get_text (GTK_ENTRY (gameTable->entryRaise));
		
		/* Затирание сообщения (после нажатие "enter") */
		gtk_entry_set_text(GTK_ENTRY (gameTable->entryRaise), "");
		
		return;
}

void funcPass() {
		return;
}

/* создаём новый контейнер hbox содержащий изображение и текст  * и возвращаем контейнер. */
GtkWidget *pngBox(gchar *png_filename)
{    
		GtkWidget *box;    
		GtkWidget *image;    /* создаём контейнер для изображения и текста */    
		
		box = gtk_box_new (FALSE, 0);   
		gtk_container_set_border_width (GTK_CONTAINER(box), 0); 
		/* определяем файл с изображением */   
		image = gtk_image_new_from_file (png_filename);
	   
		/* упаковываем рисунок в контейнер */
		gtk_box_pack_start (GTK_BOX (box), image, TRUE, FALSE, 0);   
		gtk_widget_show (image);    
	 
		return box;
}

void cell_clicked (GtkWidget *widget) //Обработка клика по изображению (event_box)
{   
		return; 
}

void destroyWidget (struct playerBox *players) {
		int index;
		for (index = 0; index < MAX_PLAYERS; index++) {
				if (players[index].cardInfoBox != NULL)
					gtk_widget_destroy(players[index].cardInfoBox);
		}
}

void initBoxPlayers (struct graf_player_t *p, struct playerBox *players) {
		GdkRGBA color = {5.0, 5.0, 5.0, 5.0};
		destroyWidget (players);
		int i; 
		int j;
	
		char path[100];
		char suitCard[2];
		char tmp_path[] = "gfx/\0";
		char format[] = ".png\0";
		
		for (i = 0; i < MAX_PLAYERS; i++) {
				if (p->enabled != 0) {
						for (j = 0; j < 100; j++)
								path[j] = '\0';
						players[i].infoBox = gtk_box_new(TRUE, 10);		
						players[i].cardInfoBox = gtk_box_new(FALSE, 0);
						players[i].cardsBox = gtk_box_new(FALSE, DISTANCE_BETWEEN_CARDS);
						
						players[i].name = gtk_label_new (p[i].name);
						players[i].money = gtk_label_new (p[i].money_text);
						players[i].bet = gtk_label_new (p[i].status_text);
						
						gtk_widget_override_color(players[i].name, 
							GTK_STATE_NORMAL, &color);  
						gtk_widget_override_color(players[i].money, 
							GTK_STATE_NORMAL, &color);  
						gtk_widget_override_color(players[i].bet, 
							GTK_STATE_NORMAL, &color);  
						
						if (i != 0) {
								gtk_box_pack_start(GTK_BOX(players[i].infoBox),
										players[i].name, FALSE, FALSE, 10);
						} else {
								gtk_box_pack_start(GTK_BOX(players[i].infoBox),
										players[i].name, FALSE, FALSE, 30);
						}
						
						players[i].leftCard = NULL;
						players[i].rightCard = NULL;
						
						for (j = 0; j < 2; j++) {
								if (p->cards->val[0] != '\0') {
										suitCard[1] = '\0';
										if ( p->cards[j].index_suit == GRAF_INDEX_SPADES) {
												suitCard[0] = '\0' + GRAF_INDEX_SPADES;
										}
										if ( p->cards[j].index_suit == GRAF_INDEX_CLUBS) {
												suitCard[0] = '\0' + GRAF_INDEX_CLUBS;
										}
										if ( p->cards[j].index_suit == GRAF_INDEX_HEARTS) {
												suitCard[0] = '\0' + GRAF_INDEX_HEARTS;
										}				
										if ( p->cards[j].index_suit == GRAF_INDEX_DIAMONDS) {
												suitCard[0] = '\0' + GRAF_INDEX_DIAMONDS;
										}				

										strncpy(path,  tmp_path, strlen(tmp_path));
										strncat(path,  p->cards[j].val , strlen(p->cards[j].val));
										strncat(path,  suitCard , strlen(suitCard));
										strncat(path,  format, strlen(format));
										
										if (j == 0) players[i].leftCard = pngBox(path); 
										if (j == 1) players[i].rightCard = pngBox(path);
								}
						}
						gtk_box_pack_start(GTK_BOX(players[i].infoBox),
								players[i].name, FALSE, FALSE, 0);
						gtk_box_pack_start(GTK_BOX(players[i].infoBox),
								players[i].money, FALSE, FALSE, 0);
						gtk_box_pack_start(GTK_BOX(players[i].infoBox),
								players[i].bet, FALSE, FALSE, 0);
						
						if (players[i].leftCard == NULL &&
							players[i].rightCard == NULL) {
								players[i].leftCard = pngBox("gfx/rubashka.png");
								players[i].rightCard = pngBox("gfx/rubashka.png");
						}
						
						// Отображение хода текущего игрока 
						if (p->selected == 1) {
								if (gameTable->players[i].imgBox != NULL)
										gtk_widget_destroy(gameTable->players[i].imgBox);
								gameTable->players[i].imgBox = pngBox("gfx/playerActive.png");
								gtk_container_add (GTK_CONTAINER (gameTable->players[i].box), 
										gameTable->players[i].imgBox);
								gtk_widget_show_all(gameTable->players[i].box);
						}
						
						gtk_box_pack_start(GTK_BOX(players[i].cardsBox), 
							players[i].leftCard, FALSE, FALSE, 0);
						gtk_box_pack_start(GTK_BOX(players[i].cardsBox),
							players[i].rightCard, FALSE, FALSE, 0);
						
						gtk_box_pack_start(GTK_BOX(players[i].cardInfoBox),
							players[i].cardsBox, FALSE, FALSE, 
							DISTANCE_BETWEEN_CARDS_MAIN_BOX);
						  
						gtk_box_pack_start(GTK_BOX(players[i].cardInfoBox),
							players[i].infoBox, FALSE, FALSE, 0);
							
						gtk_overlay_add_overlay (GTK_OVERLAY(players[i].box), 
							players[i].cardInfoBox);
							
						gtk_widget_hide(players[i].box);
						gtk_widget_show_all(players[i].box);
				}
		}
}

void initBoxTable (struct graf_bank_t *p,
		struct playersGameTableBox *playersGameTable) {
		
		gchar str[256] ;
		int i;
		
		strncpy(str, "Bank: \0", strlen( "Bank: \0"));
		strncat(str,  p->money_text, strlen(p->money_text));
		strncpy(str, "\n\0", strlen( "\n\0"));
		strncat(str,   "Timer: \0", strlen("Status: \0"));
		strncat(str,  p->timer_text, strlen(p->timer_text));

		gtk_label_set_text (GTK_LABEL(playersGameTable->labelInfo), str);
		
		for (i = 0; i < 5; i++) {
				
				if (p->cards->val[0] != '\0') {
						// разбор карты
				
				gtk_widget_destroy(playersGameTable->tableCard[i]);
				playersGameTable->tableCard[i] = pngBox("gfx/rubashka.png");
				gtk_box_pack_start(GTK_BOX(playersGameTable->gameTable),
						playersGameTable->tableCard[i], 
						FALSE, FALSE, DISTANCE_BETWEEN_CARDS_BANK);
				gtk_widget_show(playersGameTable->tableCard[i]);
				}
		}	
}

void createBoxPlayers(struct playerBox *players){
		
		int i;
		
		for (i = 0; i < MAX_PLAYERS; i++) {
				players[i].box = gtk_overlay_new();
				players[i].cardInfoBox = NULL;
				players[i].imgBox = pngBox("gfx/playerNotActive.png");
			
				gtk_container_add (GTK_CONTAINER (players[i].box), 
						players[i].imgBox);
		} 
}

GtkWidget *initUserPlayer(struct playerBox *players) {
		GtkWidget *box;
		
		box = gtk_box_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), players[0].box, FALSE, FALSE, 0);
		
		return box;
}

GtkWidget *initEnemyPlayer(struct playerBox *players) {
		GtkWidget *box;
		int i;
		box = gtk_box_new(FALSE, DISTANCE_BETWEEN_PLAYERS);
			
		for (i = 1; i < 4; i++)
			gtk_box_pack_start(GTK_BOX(box), players[i].box, FALSE, FALSE, 0);
		 
		return box;
}

GtkWidget *crateBoxTable(struct playersGameTableBox *p) {
		GtkWidget *box;
		GtkWidget *labelInfo;
		GdkRGBA color = {5.0, 5.0, 5.0, 5.0};
		gchar str[] = "$$$ Bank $$$";
		int i;
		
		box = gtk_box_new(FALSE, 0);
		p->gameTable = gtk_box_new(FALSE, 0);
		labelInfo = gtk_label_new(str);
		
		/* Меняем цвет шрифта */ 
		gtk_widget_override_color(labelInfo, GTK_STATE_NORMAL, &color);
		
		for (i = 0; i < 5; i++) {
			p->tableCard[i] = pngBox("gfx/flipside.png");
			gtk_box_pack_end(GTK_BOX(p->gameTable), p->tableCard[i], 
				FALSE, FALSE, DISTANCE_BETWEEN_CARDS_BANK);
		}
		gtk_box_pack_start(GTK_BOX(box), labelInfo, 
			FALSE, FALSE, 40);
		gtk_box_pack_start(GTK_BOX(box), p->gameTable,
			FALSE, FALSE, 0);
		
		return box;
}

GtkWidget *createButtoms() {
		GtkWidget *box;
		GtkWidget *buttomFold, *buttomRaise, *buttomCheck, *buttomAdopt;
		GtkWidget *imgFold, *imgRaise, *imgCheck, *imgAdopt;
		GtkWidget *reiseField;
		
		box = gtk_box_new(TRUE, 5);
		gtk_widget_set_size_request (GTK_WIDGET(box), 100, 20);
		
		buttomCheck = gtk_button_new();
		imgCheck = pngBox("gfx/check.png");
		gtk_button_set_image(GTK_BUTTON(buttomCheck), imgCheck);
		
		buttomAdopt = gtk_button_new();
		imgAdopt = pngBox("gfx/adopt.png");
		gtk_button_set_image(GTK_BUTTON(buttomAdopt), imgAdopt);
		
		reiseField = gtk_box_new(FALSE, 5);
		buttomRaise = gtk_button_new();
		imgRaise = pngBox("gfx/raise.png");
		gtk_button_set_image(GTK_BUTTON(buttomRaise), imgRaise);
		
		gameTable->entryRaise = gtk_entry_new();
		gtk_entry_set_max_length (GTK_ENTRY(gameTable->entryRaise), 7);
		
		gtk_widget_set_size_request (GTK_WIDGET(reiseField), 100, 20);
		gtk_widget_set_size_request (GTK_WIDGET(gameTable->entryRaise), 40, 20);
		
	    g_signal_connect (G_OBJECT (gameTable->entryRaise), "activate",
			G_CALLBACK (funcBet), NULL);
		g_signal_connect (G_OBJECT (buttomRaise), "clicked",
			G_CALLBACK (funcBet), NULL);
			
		g_signal_connect (G_OBJECT (buttomCheck), "clicked", 
			G_CALLBACK (funcPass), NULL);
				
		g_signal_connect (G_OBJECT (buttomAdopt), "clicked",
			 G_CALLBACK (funcPass),  NULL);

		gtk_container_add(GTK_CONTAINER(reiseField), gameTable->entryRaise);
		gtk_container_add(GTK_CONTAINER(reiseField), buttomRaise);
		
		buttomFold = gtk_button_new();
		imgFold = pngBox("gfx/fold.png");
		gtk_button_set_image(GTK_BUTTON(buttomFold), imgFold);
		
		gtk_box_pack_start(GTK_BOX(box), reiseField, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), buttomAdopt, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), buttomCheck, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), buttomFold, FALSE, FALSE, 0);
	
		return box;
}

GtkWidget *createGameField(struct playersGameTableBox *playersGameTable) {
		GtkWidget *mainBox;
		GtkWidget *mainContainer;
		GtkWidget *mainVerticalBox;
		GtkWidget *horizontalBox;
		GtkWidget *userPlayerBox;
		GtkWidget *enemyPlayerBox;
		GtkWidget *gameTableBox;
		GtkWidget *buttomsBox;
		GtkWidget *overlay, *tablePng;
		
		/* Графическая инициализация игровых ячеек */
		createBoxPlayers(playersGameTable->players);
		
		/* Упоковка созданных ячеек */
		userPlayerBox = initUserPlayer(playersGameTable->players);
		enemyPlayerBox = initEnemyPlayer(playersGameTable->players);	
		
		gameTableBox = crateBoxTable(playersGameTable);
		buttomsBox = createButtoms(playersGameTable);	
		
		mainContainer = gtk_box_new(FALSE, 0);
		mainVerticalBox = gtk_box_new(TRUE, 0);
		horizontalBox = gtk_box_new(FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(mainVerticalBox), enemyPlayerBox, 
			RESIZE_BOX_FLG, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(mainVerticalBox), gameTableBox, 
			RESIZE_BOX_FLG, FALSE, DISTANCE_BETWEEN_BANK_TOP_BOTTOM_PLAYERS);
			
		gtk_box_pack_start(GTK_BOX(horizontalBox), userPlayerBox, 
			FALSE, FALSE, 20);
		gtk_box_pack_start(GTK_BOX(horizontalBox), buttomsBox, 
			FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(mainVerticalBox), horizontalBox, 
			RESIZE_BOX_FLG, FALSE, 40);
			
		gtk_box_pack_start(GTK_BOX(mainContainer), mainVerticalBox, 
			RESIZE_BOX_FLG, FALSE, 0);

		mainBox = gtk_box_new(FALSE, 0);
		overlay = gtk_overlay_new();
		tablePng = pngBox("gfx/table3.jpeg");
		
		gtk_container_add (GTK_CONTAINER (overlay), tablePng);
		gtk_overlay_add_overlay (GTK_OVERLAY(overlay), mainContainer);
		gtk_box_pack_start(GTK_BOX(mainBox), overlay,  FALSE, FALSE, 0);
	
		return (mainBox);
}

void createMainGameWindow(struct playersGameTableBox *playersGameTable) {
		
		GtkWidget *mainContainer;

		gtk_init(NULL, NULL); 
		playersGameTable->mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_position(GTK_WINDOW(playersGameTable->mainWindow), GTK_WIN_POS_CENTER);
		gtk_window_set_title(GTK_WINDOW(playersGameTable->mainWindow), "GTK_Poker_Client");
		gtk_window_set_resizable(GTK_WINDOW(playersGameTable->mainWindow), FALSE);
		gtk_container_set_border_width(GTK_CONTAINER(playersGameTable->mainWindow), 2);
		
		mainContainer = createGameField(playersGameTable);
		
		gtk_container_add(GTK_CONTAINER(playersGameTable->mainWindow), mainContainer);   

		gtk_widget_show_all(playersGameTable->mainWindow);
		
		gtk_main();
}


void grafDrawAll(struct graf_table_t* p) {
		//Разрушить ранее отображение
		if (gameTable != NULL) 
				gtk_widget_destroy(gameTable->mainWindow);
				
		createMainGameWindow(gameTable);			
		initBoxPlayers (p->players, gameTable->players);
		initBoxTable(&(p->bank), gameTable);
}

/*
void grafDrawTimer(struct graf_table_t* p, const char *strTimer) {
		initBoxTable(&(p->bank), gameTable);
}*/
