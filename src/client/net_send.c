#include "net_header.h"

/*
* Send message to server
* return value good: < 0, error: -1
*/
int net_send(int fd, void *buffer, int type, int len)
{
    printf("net_send()\n");
    int rw_bytes;

    /* Send buffer to server */
    char msg[1024];

    memset(msg, 0, 1024);

    memcpy(msg, &type, 4);
    memcpy(msg + 4, &len, 4);
    memcpy(msg + 8, (char*)buffer, len);

    rw_bytes = write(fd, msg, 1024);
    if (rw_bytes < 0) {
        return -1;
    }
    return rw_bytes;
}
