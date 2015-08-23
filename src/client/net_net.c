#include "net_header.h"

static int fd = 0;

/*
* Close file descriptor, disconnect from server
* return value error: -1, good: 0
*/
int net_disconnect_server()
{
    int ret_val;

    /* Close file descriptor */
    ret_val = close(fd);
    if (ret_val == -1) {
	strcpy(net_error_msg, "error: can't disconnect from server\n");
	return -1;
    } else {
	return 0;
    }
}

/*
* Make system call socket() to create tcp socket
* return value error: -1, good: fd
*/
int net_create_socket()
{
    /* Create TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    return fd;
}

/*
* Create socket and connect to server
* return value good: 0, error: -1
*/
int net_create_connect_server(char *addres, int port)
{
    struct sockaddr_in addr; //server structure

    /* Check port */
    if ((port > 65000) || (port < 1024)) {
	strcpy(net_error_msg, "error: not correct port\n");
	return -1;
    }

    /* Zero out structure */
    bzero((char*)&addr, sizeof(addr));

    /* Fill server structure */
    addr.sin_family = AF_INET;     // setup family
    addr.sin_port   = htons(port); // setup port

    /* Check ip address and setup address */
    if ((inet_aton(addres, &addr.sin_addr.s_addr)) == 0) {
	strcpy(net_error_msg, "error: not correct ip address\n");
	return -1;
    }
    //addr.sin_addr.s_addr = inet_addr(addres);

    /* Create TCP socket */
    fd = net_create_socket();
    if (fd < 0) {
	strcpy(net_error_msg, "error: can't socket()\n");
        return -1;
    }

    /* Connect to server */
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	strcpy(net_error_msg, "error: can't connect() to server\n");
        return -1;
    }
    return 0;
}

/*
* Send message to server
* return value good: < 0, error: -1
*/
int net_send(void *buffer, int type, int len)
{
    int rw_bytes;

    /* Send buffer to server */
    char msg[1024];

    memset(msg, 0, 1024);

    memcpy(msg, &type, 4);
    memcpy(msg + 4, &len, 4);
    memcpy(msg + 8, (char*)buffer, len);

    rw_bytes = write(fd, msg, 1024);
    if (rw_bytes < 0) {
	strcpy(net_error_msg, "error: can't write to server\n");
        return -1;
    }
    return rw_bytes;
}


/*
* Receive message from server
* return value error: -1, good: 0
*/
int net_receive(void *buffer, int *type, int *len)
{
    int rw_bytes;
    int offset;
    struct network_msg_hdr_t *recv_msg;

    /* Zero out before read message */
    bzero((char*)buffer, 1024);

    /* Receive buffer from server */
    rw_bytes = read(fd, (char*)buffer, 1024);
    if (rw_bytes < 0) {
	strcpy(net_error_msg, "error: can't read from server\n");
        return -1;
    }

    recv_msg = (struct network_msg_hdr_t*)buffer;

    *type = recv_msg->payload_type;
    *len = *((int*)(buffer + 4));
    memmove(buffer, buffer + 8, *len);

    return 0;
}
