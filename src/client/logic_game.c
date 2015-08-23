#include "logic.h"

void run_game(char *ip, int port, int session) {
    int err = net_create_connect_server(ip, port);
    if (err < 0) {
        printf("Server unavaible\n");
        exit(1);
    }

    /* Отправка сессии серверу
    */
    net_send(&session, ACTION_CONNECT_REQUEST, sizeof(session));
}
