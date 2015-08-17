#include "net_header.h"

/*
* Close file descriptor, disconnect from server
* no return value
*/
void net_disconnect_server(int fd)
{
    /* Close file descriptor */
    close(fd);
}