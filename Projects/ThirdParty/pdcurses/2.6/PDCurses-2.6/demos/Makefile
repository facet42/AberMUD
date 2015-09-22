# Generated automatically from Makefile.in by configure.
# Makefile for XCurses demo programs.

SHELL		= /bin/sh

srcdir		= ../../PDCurses-2.6-src/demos
VPATH		= ../../PDCurses-2.6-src/demos
prefix		= c:/progra~1/PDCurses
exec_prefix	= ${prefix}
libdir		= $(exec_prefix)
PDCURSES_HOME		=$(srcdir)/..
PDCURSES_CURSES_H		=$(PDCURSES_HOME)/curses.h

CC		= gcc
CFLAGS		= -O3 -Wall -fomit-frame-pointer

INCDIR		= $(srcdir)
CPPFLAGS	= -I$(INCDIR) -I$(srcdir)/.. -I.. -DHAVE_CONFIG_H 

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -I/usr/X11R6/include/X11 -I/usr/X11R6/include/X11 -I/usr/X11R6/include

LIB_DEPS	=	-L../pdcurses -lcurses
LIBXCURSES	=	../pdcurses/libcurses.a

PANEL_LIB	=	-L../panel -lpanel
LIBPANEL	=	../panel/libpanel.a

LINK		=  $(CC)
LDFLAGS		= $(LIB_DEPS)  -Wl,-s -Wl,--force-exe-suffix -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition    -Wl,-s  -Le:/progra~1/gnuwin32/lib -lgw32c  -liberty -lintl -liconv -lwsock32 -lole32 -luuid -lmsvcp60   

EXEEXT		= .exe

DEMOS = \
	firework$(EXEEXT) \
	newdemo$(EXEEXT) \
	ptest$(EXEEXT) \
	testcurs$(EXEEXT) \
	tuidemo$(EXEEXT) \
	xmas$(EXEEXT) 

all: $(DEMOS) 

$(LIBXCURSES):
	cd ../pdcurses; $(MAKE) $(MFLAGS)

$(LIBPANEL):
	cd ../panel; $(MAKE) $(MFLAGS)

firework$(EXEEXT): firework.o $(LIBXCURSES)
	$(LINK) firework.o -o $@ $(LDFLAGS)

newdemo$(EXEEXT): newdemo.o $(LIBXCURSES)
	$(LINK) newdemo.o -o $@ $(LDFLAGS)

ptest$(EXEEXT): ptest.o $(LIBXCURSES) $(LIBPANEL)
	$(LINK) ptest.o -o $@ $(PANEL_LIB) $(LDFLAGS)

testcurs$(EXEEXT): testcurs.o $(LIBXCURSES)
	$(LINK) testcurs.o -o $@ $(LDFLAGS)

tuidemo$(EXEEXT): tuidemo.o tui.o $(LIBXCURSES)
	$(LINK) tui.o tuidemo.o -o $@ $(LDFLAGS)

xmas$(EXEEXT): xmas.o $(LIBXCURSES)
	$(LINK) xmas.o -o $@ $(LDFLAGS)


firework.o: $(srcdir)/firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/firework.c

newdemo.o: $(srcdir)/newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/newdemo.c

ptest.o: $(srcdir)/ptest.c $(PDCURSES_HOME)/panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/ptest.c

testcurs.o: $(srcdir)/testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/testcurs.c

tui.o: $(srcdir)/tui.c $(srcdir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/tui.c

tuidemo.o: $(srcdir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/tuidemo.c

xmas.o: $(srcdir)/xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(srcdir)/xmas.c

install:
	echo Does nothing at the moment

clean:
	-rm -rf *.o trace $(DEMOS)

distclean: clean
	-rm -f Makefile

mostlyclean: clean

realclean: distclean

#
# This section provides for compiling and linking the
# ncurses test programs.
#
ncurses_testdir=/home/mark/ncurses-1.9.6/test

NCURSES_TESTS = \
	bs \
	gdc \
	hanoi \
	knight \
	rain \
	tclock \
	view \
	worm \
	ncurses

ncurses_tests: $(NCURSES_TESTS)

bs: bs.o $(LIBXCURSES)
	$(LINK) bs.o -o $@ $(LDFLAGS)

gdc: gdc.o $(LIBXCURSES)
	$(LINK) gdc.o -o $@ $(LDFLAGS)

hanoi: hanoi.o $(LIBXCURSES)
	$(LINK) hanoi.o -o $@ $(LDFLAGS)

knight: knight.o $(LIBXCURSES)
	$(LINK) knight.o -o $@ $(LDFLAGS)

rain: rain.o $(LIBXCURSES)
	$(LINK) rain.o -o $@ $(LDFLAGS)

tclock: tclock.o $(LIBXCURSES)
	$(LINK) tclock.o -o $@ $(LDFLAGS) -lm

view: view.o $(LIBXCURSES)
	$(LINK) view.o -o $@ $(LDFLAGS)

worm: worm.o $(LIBXCURSES)
	$(LINK) worm.o -o $@ $(LDFLAGS)

ncurses: ncurses.o $(LIBXCURSES)
	$(LINK) ncurses.o -o $@ $(LDFLAGS)

bs.o: $(ncurses_testdir)/bs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/bs.c

gdc.o: $(ncurses_testdir)/gdc.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/gdc.c

hanoi.o: $(ncurses_testdir)/hanoi.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/hanoi.c

knight.o: $(ncurses_testdir)/knight.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/knight.c

rain.o: $(ncurses_testdir)/rain.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/rain.c

tclock.o: $(ncurses_testdir)/tclock.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/tclock.c

view.o: $(ncurses_testdir)/view.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/view.c

worm.o: $(ncurses_testdir)/worm.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/worm.c

ncurses.o: $(ncurses_testdir)/ncurses.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o $@ $(ncurses_testdir)/ncurses.c

#
# This section provides for compiling and linking the
# cdk example programs.
#
cdk_homedir	=/tmp/cdk-4.6.0
cdk_exampledir	=$(cdk_homedir)/examples
CDK_INC	=-I$(cdk_homedir)
CDK_LIB	=-L$(cdk_homedir) -lcdk

CDK_TESTS = \
	dialog     \
	entry      \
	fselect    \
	graph      \
	histogram  \
	itemlist   \
	label      \
	marquee    \
	matrix     \
	mentry     \
	menu       \
	radio      \
	scale      \
	scroll     \
	selection  \
	subwindow  \
	swindow    \
	template   \
	viewer

cdk_tests: $(CDK_TESTS)

dialog: dialog.o $(LIBXCURSES)
	$(LINK) dialog.o -o $@ $(CDK_LIB) $(LDFLAGS)

entry: entry.o $(LIBXCURSES)
	$(LINK) entry.o -o $@ $(CDK_LIB) $(LDFLAGS)

fselect: fselect.o $(LIBXCURSES)
	$(LINK) fselect.o -o $@ $(CDK_LIB) $(LDFLAGS)

graph: graph.o $(LIBXCURSES)
	$(LINK) graph.o -o $@ $(CDK_LIB) $(LDFLAGS)

histogram: histogram.o $(LIBXCURSES)
	$(LINK) histogram.o -o $@ $(CDK_LIB) $(LDFLAGS)

itemlist: itemlist.o $(LIBXCURSES)
	$(LINK) itemlist.o -o $@ $(CDK_LIB) $(LDFLAGS)

label: label.o $(LIBXCURSES)
	$(LINK) label.o -o $@ $(CDK_LIB) $(LDFLAGS)

marquee: marquee.o $(LIBXCURSES)
	$(LINK) marquee.o -o $@ $(CDK_LIB) $(LDFLAGS)

matrix: matrix.o $(LIBXCURSES)
	$(LINK) matrix.o -o $@ $(CDK_LIB) $(LDFLAGS)

mentry: mentry.o $(LIBXCURSES)
	$(LINK) mentry.o -o $@ $(CDK_LIB) $(LDFLAGS)

menu: menu.o $(LIBXCURSES)
	$(LINK) menu.o -o $@ $(CDK_LIB) $(LDFLAGS)

radio: radio.o $(LIBXCURSES)
	$(LINK) radio.o -o $@ $(CDK_LIB) $(LDFLAGS)

scale: scale.o $(LIBXCURSES)
	$(LINK) scale.o -o $@ $(CDK_LIB) $(LDFLAGS)

scroll: scroll.o $(LIBXCURSES)
	$(LINK) scroll.o -o $@ $(CDK_LIB) $(LDFLAGS)

selection: selection.o $(LIBXCURSES)
	$(LINK) selection.o -o $@ $(CDK_LIB) $(LDFLAGS)


dialog.o: $(cdk_exampledir)/dialog_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/dialog_ex.c

entry.o: $(cdk_exampledir)/entry_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/entry_ex.c

fselect.o: $(cdk_exampledir)/fselect_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/fselect_ex.c

graph.o: $(cdk_exampledir)/graph_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/graph_ex.c

histogram.o: $(cdk_exampledir)/histogram_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/histogram_ex.c

itemlist.o: $(cdk_exampledir)/itemlist_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/itemlist_ex.c

label.o: $(cdk_exampledir)/label_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/label_ex.c

marquee.o: $(cdk_exampledir)/marquee_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/marquee_ex.c

matrix.o: $(cdk_exampledir)/matrix_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/matrix_ex.c

mentry.o: $(cdk_exampledir)/mentry_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/mentry_ex.c

menu.o: $(cdk_exampledir)/menu_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/menu_ex.c

radio.o: $(cdk_exampledir)/radio_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/radio_ex.c

scale.o: $(cdk_exampledir)/scale_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/scale_ex.c

scroll.o: $(cdk_exampledir)/scroll_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/scroll_ex.c

selection.o: $(cdk_exampledir)/selection_ex.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(CDK_INC) -o $@ $(cdk_exampledir)/selection_ex.c

#
# This section provides for compiling and linking the
# saa example program.
#
saa_homedir	=../saa
SAA_INC	=-I$(saa_homedir)
SAA_LIB	=-L$(saa_homedir) -lsaa

fhterm: fhterm.o $(LIBXCURSES)
	$(LINK) fhterm.o -o $@ $(SAA_LIB) $(LDFLAGS)

fhterm.o: $(saa_homedir)/fhterm.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -DUSE_PDCURSES $(SAA_INC) -o $@ $(saa_homedir)/fhterm.c
