ACLOCAL_AMFLAGS = -I config
AM_CFLAGS = -I$(srcdir)/src/include -g -Wall

#
# Test program(s)
#
bin_PROGRAMS = mmap-test 
mmap_test_SOURCES = src/mmap-test.c src/mapped-ram-file.c src/shmem.c
#mmap_test_LDADD = src/libcforall.la

#pkglib_LTLIBRARIES =

EXTRA_DIST = README LICENSE TODO 

# vim: ts=8 sw=8
