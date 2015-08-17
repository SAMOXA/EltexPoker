#include "net_header.h"

/*
* Thread handler for communicate with server (only send msg)
*/
void *handler_send(void *args)
{
/* 
    int socket_fd = *((int*)args);

	char buffer[256];
	strcpy(buffer, "connect");

    // do something

    while(1) {
        int rw_bytes;
        // do something
        switch (clientMessagesTypes)
        {
            case ACTION_EXIT:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            case ACTION_CALL:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            case ACTION_RISE:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            case ACTION_FOLD:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            case ACTION_CHECK:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            case ACTION_ALL_IN:
                // do something
                // send message
                rw_bytes = net_send(socket_fd, buffer, 256);
                if (rw_bytes < 0) {
                    fprintf(stderr,"\033[0;31merror\033[0m: write()\n");
                    exit(1);
                }
                break;

            default:
                break;
        }
    }
*/    
}