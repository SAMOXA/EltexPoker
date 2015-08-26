bin_PROGRAMS = clientgtk
clientgtk_SOURCES = gtk_poker_func.c gtk_poker_main.c graf_api.h graf_list.h gtk_header.h
AM_CFLAGS = -Wall -I$(top_builddir)/src -I$(top_builddir)/src/client -I$(top_builddir)/src/grafGtk

AM_CFLAGS += @GTK_CFLAGS@
LDADD=@GTK_LIBS@
