bin_PROGRAMS = pokerClient
pokerClient_SOURCES = main.c
pokerClient_SOURCES += global.h
pokerClient_SOURCES += logic_menu.c logic.h logic_game.c network.h network.c
#pokerClient_SOURCES += graf/graf.c graf/graf.h graf/graf_api.h graf/graf_list.c graf/graf_list.h

pokerClient_LDADD = @CURSES_LIB@
LIBS = $(PTHREAD_LIBS)
AM_CFLAGS = $(PTHREAD_CFLAGS)
AM_CFLAGS += -Wall -I$(top_builddir)/src -I$(top_builddir)/src/client -I$(top_builddir)/src/client/graf
#CC = $(PTHREAD_CC)

