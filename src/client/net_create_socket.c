#include "net_header.h"

/*
* Make system call socket() to create tcp socket
* return value -1 - error, good -fd
*/
int net_create_socket(int fd)
{
    /* Create TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    return fd;
}