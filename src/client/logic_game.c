#include "logic.h"

void run_game(char *ip, int port, int session) {
    int err = net_create_connect_server(ip, port);
    if (err < 0) {
        printf("Server unavaible\n");
        exit(1);
    }
    //pthread_create()

    /* Отправка сессии серверу 
    */
    net_send(&session, ACTION_CONNECT_REQUEST, sizeof(session));
}



/*

void *handler_recv(void *args)
{

    int socket_fd = *((int*)args);

    while(1) {
	char buffer[1024];
	int ret_val;
	struct msg *recv_msg;

        ret_val = net_receive(socket_fd, &buffer, 1024);
        if (ret_val == -1) {
            fprintf(stderr,"\033[0;31merror\033[0m: read()\n");
            exit(1);
        }
	recv_msg = (struct msg*)buffer;
        // do something
        switch  (recv_msg->type)
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

	printf("Ready game tables:\n");
	printf("%s\nChoose: ", ptr);
	char input[256];
	scanf("%s", input);
}

*/
