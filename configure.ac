AC_INIT([crctk], [0.4.4], [dev@2ion.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC
AC_CONFIG_SRCDIR([src/crctk.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
PKG_CHECK_MODULES([ZLIB], [zlib])
PKG_CHECK_MODULES([KYOTOCABINET], [kyotocabinet])
AC_OUTPUT
