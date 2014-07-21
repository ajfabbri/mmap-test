AC_PREREQ([2.67])
AC_INIT([mmap-test], [1.0], [ajfabbri@gmail.com])
AC_CONFIG_SRCDIR([src/mmap-test.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

# We're using libtool to help with shared libs 
AM_PROG_AR
LT_INIT

AC_CHECK_LIB([rt], [shm_open], [], [ AC_MSG_ERROR([*** Could not find librt ***]) ]) 

# Check for other programs
AC_PROG_CC

# Do it.
AC_OUTPUT

# vim: ts=8 et sw=4 sts=4
