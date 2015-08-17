#include "net_header.h"

/*
* Make system call connect() to connect the server
* return value: error: -1, good: 0
*/
int net_connect(int fd, struct sockaddr_in server_addr)
{
    /* Connect to server */
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    }
    return 0;
}