TESTS = coreLogic
check_PROGRAMS = $(TESTS)
coreLogic_SOURCES = test.c test.h
#coreLogic_SOURCES += $(top_builddir)/src/server/gameServer/shared.c $(top_builddir)/src/server/gameServer/shared.h
#coreLogic_SOURCES += $(top_builddir)/src/server/gameServer/win.c $(top_builddir)/src/server/gameServer/win.h
LIBS = -L. $(top_builddir)/src/server/libtestcorelogic.a
AM_CFLAGS = -Wall -I$(top_builddir)/src -I$(top_builddir)/src/server -I$(top_builddir)/src/server/lobbiServer -I$(top_builddir)/src/server/gameServer
AM_CFLAGS += -Wunused-function #static functions in modules warning
