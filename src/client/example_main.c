#include "net_implementation.h"

struct net_msg {

};

int main(int argc, char **argv)
{
	static int socket_fd;
	static int size;
	static int ret_val;

	static struct net_msg *buffer;

	static char *addres = "192.168.1.1";
	static int port = 5555;

	socket_fd = net_connect_server(addres, port);

	ret_val = net_send(socket_fd, (void*)buffer, size);
	buffer = net_recive(socket_fd, (void*)buffer, size);

	net_disconnect_server(socket_fd);

	return 0;
}