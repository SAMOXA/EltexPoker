#include "net_header.h"

/*
* Fill socket strucure
* retrun value socket_addr structure
*/

struct sockaddr_in net_fill_socket_addr(struct sockaddr_in addr, 
						char *address, int port)
{
    /* Zero out structure */
    bzero((char*)&addr, sizeof(addr));

    addr.sin_family      = AF_INET;            // setup family
    addr.sin_addr.s_addr = inet_addr(address); // setup addres
    addr.sin_port        = htons(port);        // setup port

    return addr;
}