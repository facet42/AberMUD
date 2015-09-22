# Generated automatically from Makefile.in by configure.
# Makefile for XCurses manext program.

SHELL		= /bin/sh

srcdir		= ../../PDCurses-2.6-src/tools
pdcdir		= $(srcdir)/../x11

VPATH		= ../../PDCurses-2.6-src/tools
prefix		= c:/progra~1/PDCurses
exec_prefix	= ${prefix}
libdir		= $(exec_prefix)

CC		= gcc
CFLAGS		= -O3 -Wall -fomit-frame-pointer

INCDIR		= $(srcdir)
CPPFLAGS	= -DHAVE_CONFIG_H 

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) 

LINK		= $(CC)
LDFLAGS		=  -Wl,-s -Wl,--force-exe-suffix -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition    -Wl,-s  -Le:/progra~1/gnuwin32/lib -lgw32c  -liberty -lintl -liconv -lwsock32 -lole32 -luuid -lmsvcp60  

EXEEXT		= .exe

DOC_DIR=$(srcdir)/../doc
INC_DIR=$(srcdir)/..
PDCURSES_DIR=$(srcdir)/../pdcurses
PANEL_DIR=$(srcdir)/../panel

all: manual

manual: manext PDCurses.man

PDCurses.man:
	./manext $(DOC_DIR)/intro.man 			> PDCurses.man
	./manext $(DOC_DIR)/overview.man 			>> PDCurses.man
	echo .......=================================== >> PDCurses.man
	echo .......PDCurses Definitions and Variables:	>> PDCurses.man
	echo .......=================================== >> PDCurses.man
	./manext $(INC_DIR)/curses.h 	>> PDCurses.man
	echo .......============================ >> PDCurses.man
	echo .......PDCurses Functions:		>> PDCurses.man
	echo .......============================ >> PDCurses.man
	./manext $(PDCURSES_DIR)/*.c >> PDCurses.man
	echo .......============================ >> PDCurses.man
	echo .......PDCurses Private Functions:		>> PDCurses.man
	echo .......============================ >> PDCurses.man
	./manext $(pdcdir)/*.c >> PDCurses.man
	echo .......========================== >> PDCurses.man
	echo .......PDCurses Panel Functions:		>> PDCurses.man
	echo .......========================== >> PDCurses.man
	./manext $(PANEL_DIR)/*.c >> PDCurses.man
	echo .......======================== >> PDCurses.man
	echo .......X11 considerations:		>> PDCurses.man
	echo .......======================== >> PDCurses.man
	./manext $(DOC_DIR)/x11.man 			>> PDCurses.man

manext: manext.o 
	$(LINK) manext.o -o $@ $(LDFLAGS)

manext.o: $(srcdir)/manext.c
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/manext.c

install:
	echo Does nothing at the moment

clean:
	-rm -rf *.o manext$(EXEEXT) PDCurses.man

distclean: clean
	-rm -f Makefile

mostlyclean: clean

realclean: distclean
