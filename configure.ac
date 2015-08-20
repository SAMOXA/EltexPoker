#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Poker], [0.1], [amdsamoxa@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects serial-tests])
AM_MAINTAINER_MODE([disable])
AC_CONFIG_SRCDIR([src/client/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([socket])

#AC_CHECK_LIB ([lpthread], [pthread_create])
#AC_CHECK_LIB ([lncursesw], [clear])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/client/Makefile])

AC_OUTPUT
