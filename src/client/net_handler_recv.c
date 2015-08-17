#include "net_header.h"

/*
* Thread handler for communicate with server (only receive msg)
*/
void *handler_recv(void *args)
{
/*    
    int socket_fd = *((int*)args);

    while(1) {
        char *ptr = NULL;
        ptr = net_receive(socket_fd, 256);
        if (!ptr) {
            fprintf(stderr,"\033[0;31merror\033[0m: read()\n");
            exit(1);    
        }
        // do something
        switch  (serverMessagesTypes)  
        {
            case STATE_NEW_PLAYER:
                // do smething
                break;
            case STATE_PLAYER_STATE_CHANGED:
                // do smething
                break;
            case STATE_FULL_UPDATE:
                // do smething
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
        free(ptr);
    }

	printf("Ready game tables:\n");
	printf("%s\nChoose: ", ptr);
	char input[256];
	scanf("%s", input);
*/
}