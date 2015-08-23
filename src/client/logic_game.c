#include "logic.h"


struct gameState_t gameState;
struct graf_table_t grafState;
unsigned char myid;

const char *player_state[] = {
    "FREE",
    "ACTIVE",
    "PASS",
    "CONNECTING",
    "DISCONNECTED"
};


// (O_O)
void mallocCards(struct graf_table_t *grafTab) {
    int i;
    for (i = 0; i < GRAF_MAX_PLAYERS; i++) {
        grafTab->players[i].card_num = numCardsPlayer;
        grafTab->players[i].cards = (struct graf_card_t *) 
            malloc(sizeof(struct graf_card_t) * numCardsPlayer);
    }
    grafTab->bank.card_num = numCardsTable;
    grafTab->bank.cards = (struct graf_card_t *) 
            malloc(sizeof(struct graf_card_t) * numCardsTable);
}

void freeCards(struct graf_table_t *grafTab) {
    int i;
    for (i = 0; i < GRAF_MAX_PLAYERS; i++) {
        if (grafTab->players[i].cards != NULL)
            free(grafTab->players[i].cards);
    }
    if (grafTab->bank.cards != NULL)
        free(grafTab->bank.cards);
}
// (O_O)


void gameExit() {
    net_disconnect_server();
    if (graf == 1) {
        grafExit();
        graf = 0;
    }
    freeCards(&grafState);
    printf("EXIT\n");
    exit(0);
}

void convertValueCard(unsigned char card, char *s, int* lear) {
    
    if (card == FALSE_CARD) {
        strcpy(s, "");
        *lear = GRAF_INDEX_NONE;
        return;
    }
    int val = GET_CARD_VALUE(card);
    *lear = GET_CARD_LEAR(card);
    val += 2;
    if (val <= 10) {
        sprintf(s, "%d", val);
    } else {
        val -= 2;
        switch(val) {
            case JACK:
                strcpy(s, "J");
                break;
            case QUEEN:
                strcpy(s, "Q");
                break;
            case KING:
                strcpy(s, "K");
                break;
            case ACE:
               strcpy(s, "A");
                break;
        }
    }

}

void convertPlayerState(struct player_t *player, char *sstate) {
   sprintf(sstate, "%s - %d", player_state[player->state], player->bet);
} 



void convertPlayer(struct player_t *player, struct graf_player_t* grafPlayer) {
    int i; 
    grafPlayer->enabled = 1;
    grafPlayer->card_num = 2;
    // Определение id клиента
    if (myid == 0 && (strcmp(player->name, user_name) == 0)) {
        myid = player->id;
    }
    strcpy(grafPlayer->name, player->name);
    sprintf(grafPlayer->money_text, "%d", player->money);
    convertPlayerState(player, grafPlayer->status_text);
    for (i = 0; i < 2; i++) {
        convertValueCard(player->cards[i], grafPlayer->cards[i].val, 
            &(grafPlayer->cards[i].index_suit));
    }
}

void convertBank(struct graf_bank_t *grafBank, struct gameState_t *gameState) {
    int i;
    grafBank->card_num = numCardsTable;
    for (i = 0; i < numCardsTable; i++) {
        convertValueCard(gameState->cards[i], grafBank->cards[i].val, 
             &(grafBank->cards[i].index_suit));
    }
    sprintf(grafBank->money_text, "%d", gameState->bank);
}


void gameCheckMyTurn(struct gameState_t *gameState) {
    if (gameState->activePlayerId == myid) {
        char msg[GRAF_MAX_STATUS_TEXT_SIZE];
        sprintf(msg, "Min bet: %d", gameState->bet);
        grafShowInput("YOUR TURN", msg);
        //запуск таймера
    } else {
        grafHideInput();
    }
}

void gameFullUpdate(void *buf, struct gameState_t *gameState, struct graf_table_t* grafState) {
   int i;
   memcpy(gameState, buf, sizeof(struct gameState_t));
   for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) 
        convertPlayer(&(gameState->players[i]), &(grafState->players[i]));
    gameCheckMyTurn(gameState);
    convertBank(&(grafState->bank), gameState);
    grafDrawAll(grafState);
}

void gameError(void *buf, struct gameState_t *gameState, struct graf_table_t* grafState) {
    struct errorMsg_t *errMsg = (struct errorMsg_t *) buf;
    grafShowInput("", errMsg->msg); 
}



void gameHandlerListener() {

    mallocCards(&grafState);
    unsigned char buf[1024];
    while(1) {
    	int ret_val, type, len;
        ret_val = net_receive(buf, &type, &len);
        if (ret_val == -1) {
            printf("\033[0;31merror\033[0m: read()\n");
            gameExit();
        }
        // do something
        switch  (type)
        {
            case STATE_NEW_PLAYER:
                // do something
                break;
            case STATE_PLAYER_STATE_CHANGED:
                // do something
                break;
            case STATE_FULL_UPDATE:
                
                gameFullUpdate(buf, &gameState, &grafState);
                break;
            case STATE_ACTIVE_PLAYER_CHANGED:
                // do smething
                break;
            case STATE_NEW_CARD_TABLE:
                // do smething
                break;
            case STATE_NEW_CARD_PALAYER:
                // do something
                break;
            case STATE_PLAYER_DISCONECTED:
                break;
            case STATE_ERROR:
                gameError(buf, &gameState, &grafState);
                break;

        }
    }

}

void run_game(char *ip, int port, int session) {
    myid = 0;
    int err = net_create_connect_server(ip, port);
    if (err < 0) {
        printf("Server unavaible\n");
        exit(1);
    }
    graf = 1;
    /* Отправка сессии серверу 
    */
    err = net_send(&session, ACTION_CONNECT_REQUEST, sizeof(session));
    if (err < 0) {
        printf("Error: Send session\n");
        exit(1);
    }
    gameHandlerListener();
}
