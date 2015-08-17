#include "net_header.h"

/*
* Receive message from server
* return value error: NULL, good: buffer
*/
void *net_receive(int fd, int size)
{
    int rw_bytes;

    char *buffer = (char*)malloc(size);

    /* Zero out before read message */
    bzero((char*)buffer, size);

    /* Receive buffer from server */
    rw_bytes = read(fd, buffer, size);
    if (rw_bytes < 0) {
    	free(buffer);
        return NULL;
    }
    return buffer;
}