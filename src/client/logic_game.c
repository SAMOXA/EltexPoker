#include "logic.h"

void gameServer(){
    int retCode;
    void *msg;
    int msgType;
    struct errorMsg_t *err;
    int fails = 0;

    printf("Port %d session %d\n", port, session);
    do{
        retCode = initNetwork();
        fails++;
        sleep(1);
    }while(retCode != 0 && fails < 5);
    sendMsg(ACTION_CONNECT_REQUEST, sizeof(int), &session);
    msg = recvMsg(&msgType);
    if(msgType == STATE_ERROR){
        err = (struct errorMsg_t *)msg;
        printf("%s\n", err->msg);
        return;
    }
    if(startNetworkThread() != 0){
        printf("Error when create network thread\n");
        return;
    }
    printf("Sended\n");
    networkLoop();
}

void gameHandler(int type, void *msg)
{
    printf("gameHandler\n");
    switch  (type)
    {
        case STATE_NEW_PLAYER:
            printf("STATE_NEW_PLAYER\n");
            break;
        case STATE_PLAYER_STATE_CHANGED:
            printf("STATE_PLAYER_STATE_CHANGED\n");
            break;
        case STATE_FULL_UPDATE:
            printf("STATE_FULL_UPDATE\n");
            break;
        case STATE_ACTIVE_PLAYER_CHANGED:
            printf("STATE_ACTIVE_PLAYER_CHANGED\n");
            break;
        case STATE_NEW_CARD_TABLE:
            printf("STATE_NEW_CARD_TABLE\n");
            break;
        case STATE_NEW_CARD_PALAYER:
            printf("STATE_NEW_CARD_PALAYER\n");
            break;
        default:
            break;
    }
}
