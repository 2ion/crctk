#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([crctk], [0.3.3], [dev@2ion.eu])
AC_CONFIG_SRCDIR([src/crctk.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([libcdb], [libcdb])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([regcomp strdup strrchr strtol])


AC_OUTPUT(Makefile src/Makefile doc/Makefile man/Makefile)



AC_OUTPUT
