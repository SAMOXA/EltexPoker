#ifndef GTK_POKER_H
#define GKT_POKER_H

#include <gtk-3.0/gtk/gtk.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graf_api.h"

#define MAX_PLAYERS 4
#define RESIZE_BOX_FLG TRUE
#define DISTANCE_BETWEEN_CARDS 5
#define DISTANCE_BETWEEN_PLAYERS 70
#define DISTANCE_BETWEEN_USERPLAYER_MAIN_BOX 0
#define DISTANCE_BETWEEN_CARDS_BANK 5
#define DISTANCE_BETWEEN_CARDS_MAIN_BOX 20
#define DISTANCE_BETWEEN_BANK_TOP_BOTTOM_PLAYERS 20

struct playerBox {
	GtkWidget *box;
	GtkWidget *cardInfoBox;
	GtkWidget *cardsBox;
	GtkWidget *imgBox;
	GtkWidget *leftCard;
	GtkWidget *rightCard;
	GtkWidget *infoBox;
	GtkWidget *name;
	GtkWidget *money;
	GtkWidget *bet;
};

struct playersGameTableBox {
	GtkWidget *mainWindow;
	GtkWidget *tableCard[5];
	GtkWidget *labelInfo;
	GtkWidget *gameTable;
	GtkWidget *entryRaise;
	struct playerBox players[MAX_PLAYERS];
};

void funcExit();

void funcBet();

void funcPass();

GtkWidget *png_box(gchar *png_filename);

void destroyWidget (struct playerBox *players);

void initBoxPlayers (struct graf_player_t *p, struct playerBox *players);

void createBoxPlayers(struct playerBox *players);

void initBoxTable (struct graf_bank_t *p, 
	struct playersGameTableBox *playersGameTable);

GtkWidget *initUserPlayer(struct playerBox *players);

GtkWidget *initEnemyPlayer(struct playerBox *players);

GtkWidget *crateBoxTable(struct playersGameTableBox *p);

GtkWidget *createGameField(struct playersGameTableBox *playersGameTable);

GtkWidget *createButtoms();

void createMainGameWindow(struct playersGameTableBox *playersGameTable);

#endif
