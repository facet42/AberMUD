# Generated automatically from Makefile.in by configure.
# Makefile for XCurses library
#
# The variable 'srcdir' refers to the source-distribution, and can be set with
# the configure script by "--srcdir=DIR".
#

SHELL		= /bin/sh
THIS		= Makefile



srcdir		= ../../PDCurses-2.6-src/panel
VPATH		= ../../PDCurses-2.6-src/panel
prefix		= c:/progra~1/PDCurses
exec_prefix	= ${prefix}
libdir		= $(exec_prefix)
includedir	= ${prefix}/include

PDCURSES_HOME		=$(srcdir)/..
PDCURSES_CURSES_H		=$(PDCURSES_HOME)/curses.h

INSTALL		= /bin/install.exe -cp
INSTALL_DATA	= /bin/install.exe -cp

LN_S		= @LN_S@

CC		= gcc
CFLAGS		= -O3 -Wall -fomit-frame-pointer

INCDIR		= $(srcdir)
CPPFLAGS	= -I$(INCDIR) -I$(srcdir)/.. -I.. -DHAVE_CONFIG_H -DXCURSES 

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -I/usr/X11R6/include/X11 -I/usr/X11R6/include/X11 -I/usr/X11R6/include

LINK		= $(CC)
LDFLAGS		=  -Wl,-s -Wl,--force-exe-suffix -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition    -Wl,-s  -Le:/progra~1/gnuwin32/lib -lgw32c  -liberty -lintl -liconv -lwsock32 -lole32 -luuid -lmsvcp60  

RANLIB		= ranlib

LIBRARIES	= libpanel.a

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

OBJS = \
panel.o

libpanel.a : $(OBJS)
	ar rv $@ $?
	-$(RANLIB) $@

panel.o: $(srcdir)/panel.c $(PDCURSES_HOME)/panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/panel.c
