#include "net_header.h"

/*
* Send message to server
* return value good: < 0, error: -1
*/
int net_send(int fd, void *buffer, int size)
{
    int rw_bytes;

    /* Send buffer to server */
    rw_bytes = write(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
        return -1;
    }
    return rw_bytes;
}
