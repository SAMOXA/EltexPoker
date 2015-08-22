#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Poker], [0.1], [amdsamoxa@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects serial-tests])
AM_MAINTAINER_MODE([disable])
#AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/client/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

# Checks for libraries.
#AX_PTHREAD([], [AC_MSG_ERROR([requires pthread library])])
AX_WITH_CURSES
#if test "x$ax_cv_ncursesw" != xyes; then
#  AC_MSG_ERROR([requires NcursesW library])
#fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h locale.h malloc.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero inet_ntoa memmove memset select setlocale socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/client/Makefile
                 src/server/Makefile
                 tests/Makefile
                 tests/coreLogic/Makefile])

AC_OUTPUT
