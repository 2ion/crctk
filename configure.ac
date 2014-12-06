AC_INIT([crctk], [0.5.1], [dev@2ion.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CXX
AC_PROG_CC

AC_CONFIG_SRCDIR([src/crctk.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

PKG_CHECK_MODULES([ZLIB], [zlib])
PKG_CHECK_MODULES([KYOTOCABINET], [kyotocabinet])
PKG_CHECK_MODULES([NCURSESW], [ncursesw])

AH_VERBATIM([_FORTIFY_SOURCE],
[/* Fortify source in gcc */
#if __OPTIMIZE__
#define _FORTIFY_SOURCE 2
#endif
])

AC_OUTPUT
