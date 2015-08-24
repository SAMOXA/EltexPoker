#include "network.h"

char address[INET_ADDRSTRLEN];
int port;
int fd;
char sendBuf[1024];
char readBuf[1024];
pthread_t thread;

int initNetwork() {
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port        = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("Create socket");
        return -1;
    }
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect");
        return -1;
    }
    return 0;
}

int sendMsg(int type, int length, void *buf){
    struct network_msg_hdr_t msg;
    int writeBytes = 0;
    int writeSize = sizeof(struct network_msg_hdr_t) + length;
    msg.payload_type = type;
    msg.payload_len = length;
    memcpy(sendBuf, &msg, sizeof(struct network_msg_hdr_t));
    if(length != 0 && buf != NULL){
        memcpy(sendBuf + sizeof(struct network_msg_hdr_t), buf, length);
    }
    writeBytes = write(fd, (char*)sendBuf, 1024);
    //TODO writeSize != writeBytes
    return 0;
}

void *recvMsg(int *type) {
    struct network_msg_hdr_t *msg;
    int readBytes = 0;
    //readBytes = read(fd, (char*)(&msg), sizeof(struct network_msg_hdr_t));
    readBytes = read(fd, readBuf, 1024);
    msg = (struct network_msg_hdr_t *)readBuf;
    *type = msg->payload_type;
    //memcpy((char*)buf, readBuf, msg.payload_len);
    //TODO readBytes != readBytes
    return readBuf + sizeof(struct network_msg_hdr_t);
}

void *networkLoop(void *arg){
    struct network_msg_hdr_t *msg;
    int readBytes = 0;

    while(1){
        read(fd, readBuf, 1024);
        msg = (struct network_msg_hdr_t *)readBuf;
        gameHandler(msg->payload_type, readBuf + sizeof(struct network_msg_hdr_t));
    }
}

int startNetworkThread(){
    return pthread_create(&thread, NULL, &networkLoop, NULL);
}

void networkDisconnect()
{
    close(fd);
}
