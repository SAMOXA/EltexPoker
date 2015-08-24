#include <unistd.h>
#include "graf.h"
#include "graf_api.h"

int main(void){
    int indexA=0;
    int indexB=0;

    struct graf_table_t* tbl=(struct graf_table_t*)malloc(sizeof(struct graf_table_t));

    printf("Start\n");

    for(indexA=0;indexA<GRAF_MAX_PLAYERS;indexA++){
	tbl->players[indexA].enabled=1;
	strcpy(tbl->players[indexA].name,"name");
	strcpy(tbl->players[indexA].money_text,"Money: 100");
	strcpy(tbl->players[indexA].status_text,"Status: pass");
	tbl->players[indexA].card_num=2;
	tbl->players[indexA].cards=(struct graf_card_t*)malloc(sizeof(struct graf_card_t)*2);
	for(indexB=0;indexB<2;indexB++){
	    tbl->players[indexA].cards[indexB].selected=0;
	    strcpy(tbl->players[indexA].cards[indexB].val,"K");
	    tbl->players[indexA].cards[indexB].index_suit=1;
	    printf("Card done\n");
	}
    printf("Player done\n");
    }

    printf("Bank start\n");

    strcpy(tbl->bank.money_text,"Money: 1000");
    printf("Money\n");
    strcpy(tbl->bank.timer_text,"Timer: 00:00");
    printf("Timer\n");
    tbl->bank.card_num=3;
    tbl->bank.cards=(struct graf_card_t*)malloc(sizeof(struct graf_card_t)*3);
    for(indexB=0;indexB<3;indexB++){
	tbl->bank.cards[indexB].selected=0;
	strcpy(tbl->bank.cards[indexB].val,"K");
	tbl->bank.cards[indexB].index_suit=1;
        printf("Card done\n");
    }

    printf("Bank done\n");    

//    grafInit(tbl);

    grafDrawAll(tbl);
//    grafDrawAll(tbl);

//    sleep(1);
    grafShowInput(tbl,"Print your bet","100");
    for(indexA=0;indexA<10;indexA++){
	sprintf(tbl->bank.timer_text,"00:0%1d",9-indexA);
        grafDrawAll(tbl);
//        grafShowInput("Print your bet","100");

	sleep(1);
    }

    while(1){
	sleep(100);
    }

//    getchar();
//    grafHideInput();
//    getchar();

    grafExit();
    return 0;
}