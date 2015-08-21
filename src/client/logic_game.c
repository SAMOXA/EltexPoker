#include "logic.h"

void gameServer(){
    int retCode;
    void *msg;
    int msgType;
    struct errorMsg_t *err;

    retCode = initNetwork();
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
    //startGameLoop();
}

void gameHandler(int type, void *msg)
{
    switch  (type)
    {
        case STATE_NEW_PLAYER:
            // do something
            break;
        case STATE_PLAYER_STATE_CHANGED:
            // do something
            break;
        case STATE_FULL_UPDATE:
            // do something
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
        default:
            break;
    }
}
