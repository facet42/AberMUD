# Generated automatically from Makefile.in by configure.
# Makefile for XCurses-based SAA library
#
# The variable 'srcdir' refers to the source-distribution, and can be set with
# the configure script by "--srcdir=DIR".
#

SHELL		= /bin/sh
THIS		= Makefile



srcdir		= ../../PDCurses-2.6-src/saa
saadir		= $(srcdir)/../saa

VPATH		= ../../PDCurses-2.6-src/saa
prefix		= c:/progra~1/PDCurses
exec_prefix	= ${prefix}
libdir		= $(exec_prefix)
includedir	= ${prefix}/include

PDCURSES_HOME		=$(srcdir)/..
PDCURSES_CONFIG_H		=../config.h
PDCURSES_CURSES_H		=$(PDCURSES_HOME)/curses.h
PDCURSES_HEADERS		=$(PDCURSES_CONFIG_H) $(PDCURSES_CURSES_H)
SAA_HEADERS		=$(saadir)/saa.h $(saadir)/txtwin.h $(saadir)/mouse.h

INSTALL		= /bin/install.exe -cp
INSTALL_DATA	= /bin/install.exe -cp

LN_S		= @LN_S@

CC		= gcc
CFLAGS		= -O3 -Wall -fomit-frame-pointer

INCDIR		= $(srcdir)
CPPFLAGS	= -I$(INCDIR) -I$(srcdir)/.. -I.. -DHAVE_CONFIG_H -DXCURSES -DUSE_PDCURSES 

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -I/usr/X11R6/include/X11 -I/usr/X11R6/include/X11 -I/usr/X11R6/include

LINK		= $(CC)
LDFLAGS		= 

RANLIB		= ranlib

LIBRARIES	= libsaa.a

################################################################################
all :	$(LIBRARIES)

install : 
	echo Does nothing at the moment

clean :
	-rm -rf *.o trace $(LIBRARIES)

distclean: clean
	-rm -f Makefile

mostlyclean: clean

realclean: distclean

#------------------------------------------------------------------------

LIBOBJS =   \
saa.o       \
txtwin.o

libsaa.a : $(LIBOBJS)
	ar rv $@ $?
	$(RANLIB) $@

saa.o: $(srcdir)/saa.c $(saadir)/saa.h $(saadir)/txtwin.h $(saadir)/mouse.h
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/saa.c

txtwin.o: $(srcdir)/txtwin.c $(saadir)/txtwin.h
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/txtwin.c
