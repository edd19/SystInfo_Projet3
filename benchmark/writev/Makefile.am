AM_CFLAGS = -I$(top_srcdir)/lib @AM_CFLAGS@
bin_PROGRAMS = writev
writev_SOURCES = writev.c
writev_LDADD = $(top_builddir)/lib/libbenchmark.a \
		   $(top_builddir)/lib/libcopy.a \
		   $(AM_LDFLAGS)
GRAPHS = writev.csv writev2.csv lseek.csv lseek2.csv
PROG = writev


include ../lib/lib.mk
