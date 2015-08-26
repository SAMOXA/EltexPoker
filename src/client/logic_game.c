#include "logic.h"

#define NB_ENABLE 1
#define NB_DISABLE 0

struct player_t players[MAX_PLAYERS_PER_TABLE];
struct gameState_t state;
int myId;
int timerCounter = 30;
char lastMessage[100];
char tempInput[100];
int tempInputIndex = 0;
volatile int isInputActive = 0;
volatile int isNeedCount = 0;
pthread_mutex_t displayLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t inputLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t input_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t display_cv = PTHREAD_COND_INITIALIZER;
pthread_t displayThread;

void nonblock(int state)
{
    struct termios ttystate;
 
    tcgetattr(STDIN_FILENO, &ttystate);
    if (state==NB_ENABLE)
    {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        ttystate.c_lflag |= ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

char getUserInput(){
    char input = -1;
    int retVal = 0;

    while(isInputActive == 1 && input == -1 && retVal <= 0){
        retVal = read(0, &input, sizeof(char));
        usleep(10000);
    }
    return input;
}

char *getPlayerName(int id){
    int i;
    for(i=0;i<MAX_PLAYERS_PER_TABLE;i++){
        if(state.players[i].id == id){
            return state.players[i].name;
        }
    }
    return NULL;
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

void clearOutput(){
    int i;
    printf("\033[7A");
    for(i=0;i<8;i++){
        printf("\033[2K");
        printf("\033[B");
    }
    printf("\033[7A");
    fflush(0);
}

void *printState(void *arg){
    int i;
    static int isFirstRun = 1;

    if(isFirstRun != 0){
        for(i=0;i<7;i++){
            printf("\n");
        }
        isFirstRun = 0;
    }
    while(1){
        pthread_cond_wait(&display_cv, &displayLock);
        clearOutput();
        if(lastMessage[0] != '\0'){
            printf("%s", lastMessage);
            memset(lastMessage, 0, sizeof(char)*100);
        }
        printf("\n");
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
                printf("%3d ", state.players[i].money);
                printf("%5d    ", state.players[i].bet);
            }
        }
        printf("\n");
        if(isInputActive == 1){
            printf("f - fold | h - check | r -rise | c - call | a - all in | e -exit\n");
            if(isNeedCount == 0){
                printf("%d > ", timerCounter);
            }else{
                printf("%d count - %s",timerCounter, tempInput);
            }
        }else{
            printf("\n");
        }
        fflush(0);
        if(isFirstRun == 1){
            isFirstRun = 0;
        }
    }
    pthread_mutex_unlock(&displayLock);
}

void gameHandler(int type, void *msg)
{
    struct player_t *player;
    struct gameState_t *newState;
    int *id;
    char str[100];
    struct errorMsg_t *err;
    
    switch  (type)
    {
        case STATE_NEW_PLAYER:
            player = (struct player_t *)msg;
            sprintf(lastMessage, "New player connected - %s", player->name);
            break;
        case STATE_PLAYER_STATE_CHANGED:
            sprintf(lastMessage, "STATE_PLAYER_STATE_CHANGED");
            break;
        case STATE_FULL_UPDATE:
            sprintf(lastMessage, "STATE_FULL_UPDATE");
            newState = (struct gameState_t *)msg;
            memcpy(&state, newState, sizeof(struct gameState_t));
            break;
        case STATE_ACTIVE_PLAYER_CHANGED:
            sprintf(lastMessage, "STATE_ACTIVE_PLAYER_CHANGED");
            break;
        case STATE_NEW_CARD_TABLE:
            sprintf(lastMessage, "STATE_NEW_CARD_TABLE");
            break;
        case STATE_NEW_CARD_PALAYER:
            sprintf(lastMessage, "STATE_NEW_CARD_PALAYER");
            break;
        case STATE_PLAYER_DISCONECTED:
            id = (int *)msg;
            sprintf(lastMessage, "Player disconnected %d", *id);
            break;
        case STATE_ERROR:
            err = (struct errorMsg_t *)msg;
            sprintf(lastMessage, "ERROR - %s", err->msg);
            break;
        default:
            sprintf(lastMessage, "UNKNOWN_COMMAND");
            break;
    }
    if(state.activePlayerId == myId){
        pthread_mutex_lock(&inputLock);
        pthread_cond_signal(&input_cv);
        pthread_mutex_unlock(&inputLock);
    }
    pthread_mutex_lock(&displayLock);
    pthread_cond_signal(&display_cv);
    pthread_mutex_unlock(&displayLock);

}

void inputLoop(){
    char command;
    int count;
    int exitFlag = 0;
    int knownFlag;

    nonblock(NB_ENABLE);
    while(exitFlag == 0){
        pthread_cond_wait(&input_cv, &inputLock);
        isInputActive = 1;
        timerCounter = 30;
        while(isInputActive == 1){
            pthread_mutex_lock(&displayLock);
            pthread_cond_signal(&display_cv);
            pthread_mutex_unlock(&displayLock);
            command = getUserInput();
            knownFlag = 1;
            switch(command){
                case('f'):
                    sendMsg(ACTION_FOLD, 0, NULL);
                break;
                case('h'):
                    sendMsg(ACTION_CHECK, 0, NULL);
                break;
                case('r'):
                    isNeedCount = 1;
                    tempInputIndex = 0;
                    memset(tempInput, 0, sizeof(char)*100);
                    while(isNeedCount == 1){
                        pthread_mutex_lock(&displayLock);
                        pthread_cond_signal(&display_cv);
                        pthread_mutex_unlock(&displayLock);
                        count = getUserInput();
                        switch(count){
                            case(-1):
                                isInputActive = -1;
                                isNeedCount = -1;
                                sprintf(lastMessage, "Input timeout");
                            break;
                            case('0'):
                            case('1'):
                            case('2'):
                            case('3'):
                            case('4'):
                            case('5'):
                            case('6'):
                            case('7'):
                            case('8'):
                            case('9'):
                                tempInput[tempInputIndex] = count;
                                tempInputIndex++;
                            break;
                            case('\n'):
                                tempInput[tempInputIndex] = '\0';
                                isNeedCount = 0;
                                count = atoi(tempInput);
                            break;
                        }
                    }
                    if(count != -1){
                        sendMsg(ACTION_RISE, sizeof(int), &count);
                    }
                break;
                case('c'):
                    sendMsg(ACTION_CALL, 0, NULL);
                break;
                case('a'):
                    sendMsg(ACTION_ALL_IN, 0, NULL);
                break;
                case('e'):
                    sendMsg(ACTION_EXIT, 0, NULL);
                    exitFlag = 1;
                break;
                case(-1):
                    isInputActive = -1;
                    isNeedCount = -1;
                    knownFlag = 0;
                    sprintf(lastMessage, "Input timeout");
                    sendMsg(ACTION_FOLD, 0, NULL);
                break;
                default:
                    knownFlag = 0;
                    printf("Wrong input\n> ");
            }
            if(knownFlag != 0){
                isInputActive = 0;
                isNeedCount = 0;
            }
        }
    }
    nonblock(NB_ENABLE);
    pthread_mutex_unlock(&inputLock);
}

static void sigalrmHandler(int sig) {
    if(isInputActive == 1){
        timerCounter--;
        if(timerCounter == 0){
            isInputActive = 0;
            isNeedCount = 0;
        }
    }
    pthread_mutex_lock(&displayLock);
    pthread_cond_signal(&display_cv);
    pthread_mutex_unlock(&displayLock);
}

void startDisplayThread(){
    return pthread_create(&displayThread, NULL, &printState, NULL);
}

void gameServer(){
    int retCode;
    void *msg;
    int msgType;
    struct errorMsg_t *err;
    int fails = 0;
    int i;
    struct itimerval itv;
    struct sigaction sa;

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
    fflush(0);
    pthread_mutex_lock(&inputLock);
    pthread_mutex_lock(&displayLock);
    startDisplayThread();
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;
    sigaction(SIGALRM, &sa, NULL);
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);
    startNetworkThread();
    inputLoop();
}