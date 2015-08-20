#$(top_builddirs)
bin_PROGRAMS = pokerClient
pathgraf=../graf
LDADD = -lpthread
LDADD += -lncurses 
pokerClient_SOURCES = main.c
pokerClient_SOURCES += ../global.h
pokerClient_SOURCES += logic_menu.c logic.h net_header.h net_connect.c net_create_connect_server.c net_disconnect.c net_fill_socket_addr.c net_create_socket.c net_send.c net_receive.c 
pokerClient_SOURCES += ../graf/graf.c ../graf/graf.h ../graf/graf_api.h
