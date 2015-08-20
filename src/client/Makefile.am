#$(top_builddirs)
bin_PROGRAMS = pokerClient
pathgraf=../graf
LDADD = -lpthread
LDADD += -lncursesw
pokerClient_SOURCES = main.c
pokerClient_SOURCES += ../global.h
<<<<<<< HEAD
pokerClient_SOURCES += logic_menu.c logic.h net_header.h net_connect.c net_create_connect_server.c net_disconnect.c net_fill_socket_addr.c net_create_socket.c net_send.c net_receive.c 
pokerClient_SOURCES += ../graf/graf.c ../graf/graf.h ../graf/graf_api.h ../graf/graf_list.c ../graf/graf_list.h
=======
pokerClient_LDADD = @CURSES_LIB@
LIBS = $(PTHREAD_LIBS)
AM_CFLAGS = $(PTHREAD_CFLAGS)
#CC = $(PTHREAD_CC)
>>>>>>> 6ed1549956dc87c1ccd70bd80e561a2a93908823
