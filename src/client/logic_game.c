#include "logic.h"


struct gameState_t gameState;
struct graf_table_t grafState;
unsigned char myid;
int cur_pos, flgturn;
pthread_mutex_t mut;


const char *player_state[] = {
    "FREE",
    "ACTIVE",
    "PASS",
    "CONNECTING",
    "DISCONNECTED"
};




void gameExit() {
    net_disconnect_server();
    if (graf == 1) {
        grafExit();
        graf = 0;
    }
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


int gameCheckMyTurn(struct gameState_t *gameState) {
    if (gameState->activePlayerId == myid) {

        return 1;
    } else {
        return 0;
    }
}

void gameFullUpdate(void *buf, struct gameState_t *gameState, struct graf_table_t* grafState) {
   int i;
   char msg[GRAF_MAX_STATUS_TEXT_SIZE];

   memcpy(gameState, buf, sizeof(struct gameState_t));
   for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++)
        convertPlayer(&(gameState->players[i]), &(grafState->players[i]));
    if (gameCheckMyTurn(gameState)) {
        sprintf(msg, "Min bet: %d", gameState->bet);
        pthread_mutex_lock(&mut);
        grafShowInput("YOUR TURN", msg);
        pthread_mutex_unlock(&mut);
    } else {
        //grafHideInput();
    }
    convertBank(&(grafState->bank), gameState);
    grafDrawAll(grafState);
}

void gameError(void *buf, struct gameState_t *gameState, struct graf_table_t* grafState) {
    struct errorMsg_t *errMsg = (struct errorMsg_t *) buf;
    grafShowInput("", errMsg->msg);
}

int findMyPos(struct player_t *players) {
    int i;
    for (i = 0; i < MAX_PLAYERS_PER_TABLE; i++) {
        if (players[i].id == myid)
            return i;
    }
    return -1;
}

void gameEventBet(int sum) {
    if (gameState.activePlayerId != myid)
        return;
    int mypos = findMyPos(gameState.players);
    if (mypos == -1) {
        printf("Меня нету в списке (O_o)\n");
        gameExit();
    }
    unsigned int curMyMoney = gameState.players[mypos].money;
    if (sum < gameState.bet) {
        //Если не хватает денег, то ALL_IN
        if (curMyMoney <= gameState.bet) {
            net_send(NULL, ACTION_ALL_IN, 0);
            return;
        } else {
            pthread_mutex_lock(&mut);
            grafShowInput("Your bet is too low", "");
            pthread_mutex_unlock(&mut);
        }
    }
    // CHECK
    if (gameState.bet == sum && sum <= curMyMoney) {
        net_send(NULL, ACTION_CHECK, 0);
        return;
    }
    //BET 
    else {
        sum -= gameState.bet;
        net_send(&sum, ACTION_RISE, sizeof(sum));
        return;
    }
    printf("Что-то пошло не так с ф-ей: gameEventBet(int sum)\n");
}

void gameEventFold() {
    net_send(NULL, ACTION_FOLD, 0);
}

void initGrafGameFunc() {
    graf_exit_event = gameExit;
    graf_bet_event = gameEventBet;
    graf_pass_event = gameEventFold;
}



void gameHandlerListener() {

    unsigned char buf[1024];
    while(1) {
    	int ret_val, type, len;
        printf("before RECV\n");
        fflush(stdout);
        ret_val = net_receive(buf, &type, &len);
        if (ret_val == -1) {
            printf("\033[0;31merror\033[0m: read()\n");
            gameExit();
        }
        printf("Recv: type = %d\n", type);
        fflush(stdout);
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
    cur_pos = -1;
    initGrafGameFunc();
    pthread_mutex_init(&mut, NULL);

    int err = net_create_connect_server(ip, port);
    printf("port = %d\n", port);
    fflush(stdout);
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


