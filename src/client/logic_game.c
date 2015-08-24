#include "logic.h"

struct player_t players[MAX_PLAYERS_PER_TABLE];
struct gameState_t state;
int myId;

char *getPlayerName(int id){
    int i;
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].id == id){
            return state.players[i].name;
        }
    }
}

void printGameState(){
    switch(state.state) {
        case(GAME_START):
            printf("GAME_START ");
        break;
        case(GAME_PRE_FLOP_ROUND):
            printf("GAME_PRE_FLOP_ROUND ");
        break;
        case(GAME_FLOP_ROUND):
            printf("GAME_FLOP_ROUND ");
        break;
        case(GAME_TERN_ROUND):
            printf("GAME_TERN_ROUND ");
        break;
        case(GAME_RIVER_ROUND):
            printf("GAME_RIVER_ROUND" );
        break;
        case(GAME_FINAL):
            printf("GAME_FINAL ");
        break;
        case(GAME_INSTANT_WIN):
            printf("GAME_INSTANT_WIN ");
        break;
    }
}

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
    printf("%d %d %d ", state.bank, myId, state.activePlayerId);
    printGameState();
    printf("\n");
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].state == PLAYER_ACTIVE || 
            state.players[i].state == PLAYER_PASS){
            printf("%8s(%d) ", state.players[i].name, state.players[i].id);
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
            printf("%5d    ", state.players[i].bet);
        }
    }
    printf("\n");
}

void gameHandler(int type, void *msg)
{
    struct player_t *player;
    struct gameState_t *newState;
    int *id;
    char command;
    char str[100];
    char count;
    struct errorMsg_t *err;
    
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
        case STATE_PLAYER_DISCONECTED:
            id = (int *)msg;
            //printf("Player disconnected %s\n", getPlayerName(*id));
            printf("Player disconnected %d\n", *id);
            break;
        case STATE_ERROR:
            err = (struct errorMsg_t *)msg;
            printf("ERROR - %s\n", err->msg);
            break;
        default:
            break;
    }
    if(state.activePlayerId == myId){
        printf("0 - fold 1 - check 2 -rise 3 - call 4 - all 5 -exit\n");
        printf("> ");
        scanf("%d", &command);
        printf("%d\n", command);
        switch(command){
            case(0):
                printf("ACTION_FOLD\n");
                sendMsg(ACTION_FOLD, 0, NULL);
            break;
            case(1):
                printf("ACTION_CHECK\n");
                sendMsg(ACTION_CHECK, 0, NULL);
            break;
            case(2):
                printf("Count - ");
                scanf("%d", &count);
                printf("ACTION_RISE\n");
                sendMsg(ACTION_RISE, sizeof(int), &count);
            break;
            case(3):
                printf("ACTION_CALL\n");
                sendMsg(ACTION_CALL, 0, NULL);
            break;
            case(4):
                printf("ACTION_ALL_IN\n");
                sendMsg(ACTION_ALL_IN, 0, NULL);
            break;
            case(5):
                printf("ACTION_EXIT\n");
                sendMsg(ACTION_EXIT, 0, NULL);
            break;
            default:
                printf("> ");
        }
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
