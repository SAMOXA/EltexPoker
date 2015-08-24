#include "logic.h"

struct player_t players[MAX_PLAYERS_PER_TABLE];
struct gameState_t state;
int myId;

void printCard(unsigned char card){
    char lear = GET_CARD_LEAR(card);
    char value = GET_CARD_VALUE(card);
    if(card == FALSE_CARD){
        printf("XX ");
        return;
    }
    switch(value){
        case(TWO):
        case(THREE):
        case(FOUR):
        case(FIVE):
        case(SIX):
        case(SEVEN):
        case(EIGHT):
        case(NINE):
        case(TEN):
            printf("%d", value+2);
            break;
        case(JACK):
            printf("J");
            break;
        case(QUEEN):
            printf("Q");
            break;
        case(KING):
            printf("K");
            break;
        case(ACE):
            printf("A");
            break;
    }
    switch(lear){
        case(HEARTS):
            printf("h");
            break;
        case(DIAMONDS):
            printf("d");
            break;
        case(CLUBS):
            printf("c");
            break;
        case(SPADES):
            printf("s");
            break;
    }
    printf(" ");
}

void printState(){
    int i;
    for(i=0;i<5;i++){
        printCard(state.cards[i]);
    }
    printf("%d %d %d", state.bank, myId, state.activePlayerId);
    printf("\n");
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].state == PLAYER_ACTIVE || 
            state.players[i].state == PLAYER_PASS){
            printf("%10s ", state.players[i].name);
        }
    }
    printf("\n");
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].state == PLAYER_ACTIVE || 
            state.players[i].state == PLAYER_PASS){
            printf("     ");
            printCard(state.players[i].cards[0]);
            printCard(state.players[i].cards[1]);
        }
    }
    printf("\n");
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].state == PLAYER_ACTIVE || 
            state.players[i].state == PLAYER_PASS){
            printf("%4d ", state.players[i].money);
            printf("%4d ", state.players[i].bet);
        }
    }
    printf("\n");
}

void gameHandler(int type, void *msg)
{
    struct player_t *player;
    struct gameState_t *newState;
    
    switch  (type)
    {
        case STATE_NEW_PLAYER:
            player = (struct player_t *)msg;
            printf("New player connected - %s\n", player->name);
            //printf("STATE_NEW_PLAYER\n");
            break;
        case STATE_PLAYER_STATE_CHANGED:
            printf("STATE_PLAYER_STATE_CHANGED\n");
            break;
        case STATE_FULL_UPDATE:
            //printf("STATE_FULL_UPDATE\n");
            newState = (struct gameState_t *)msg;
            memcpy(&state, newState, sizeof(struct gameState_t));
            printState();
            break;
        case STATE_ACTIVE_PLAYER_CHANGED:
            printf("STATE_ACTIVE_PLAYER_CHANGED\n");
            break;
        case STATE_NEW_CARD_TABLE:
            printState();
            //printf("STATE_NEW_CARD_TABLE\n");
            break;
        case STATE_NEW_CARD_PALAYER:
            printState();
            //printf("STATE_NEW_CARD_PALAYER\n");
            break;
        default:
            break;
    }
    if(state.activePlayerId == myId){
        printf("TYPE\n");
    }
}

void gameServer(){
    int retCode;
    void *msg;
    int msgType;
    struct errorMsg_t *err;
    int fails = 0;
    int i;

    printf("Port %d session %d id %d\n", port, session, myId);
    do{
        retCode = initNetwork();
        fails++;
        sleep(1);
    }while(retCode != 0 && fails < 5);

    memset(&state, 0 , sizeof(struct gameState_t));

    sendMsg(ACTION_CONNECT_REQUEST, sizeof(int), &session);
    msg = recvMsg(&msgType);
    if(msgType == STATE_ERROR){
        err = (struct errorMsg_t *)msg;
        printf("%s\n", err->msg);
        return;
    }else{
        gameHandler(msgType, msg);
    }
    if(startNetworkThread() != 0){
        printf("Error when create network thread\n");
        return;
    }
    networkLoop();
}
