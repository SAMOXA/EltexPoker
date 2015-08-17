#include "net_header.h"

/*
* Receive message from server
* return value error: -1, good: 0
*/

struct msg {
    int type;
    int len;
};

int net_receive(int fd, void *buffer, int *type)
{
    int rw_bytes;
    struct msg *recv_msg;
    /* Zero out before read message */
    bzero((char*)buffer, 1024);

    /* Receive buffer from server */
    rw_bytes = read(fd, (char*)buffer, 1024);
    if (rw_bytes < 0) {
        return -1;
    }

    recv_msg = (struct msg*)buffer;

    *type = recv_msg->type;

    int len = *((int*)(buffer + 4));
    printf("\nlen = %d\n", len);
    memmove(buffer, buffer + 8, len);
    return 0;
}