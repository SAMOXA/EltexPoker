#include "net_header.h"

/*
* Receive message from server
* return value error: -1, good: 0
*/
int net_receive(int fd, void *buffer, int size)
{
    int rw_bytes;

    /* Zero out before read message */
    bzero((char*)buffer, size);

    /* Receive buffer from server */
    rw_bytes = read(fd, (char*)buffer, size);
    if (rw_bytes < 0) {
        return -1;
    }
    return 0;
}