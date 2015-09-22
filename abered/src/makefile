# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                         MAKEFILE for abered                                 #
#                                                                             #
#        Specify which version of abered you wish to try and build.           #
#                                                                             #
#           Copyright (C) 1993 James Willie. All rights reserved.             #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

def: 
	makeuse.bat

pcc286:
	echo Building a 80286 version of abered with pcc. (very memory limited)
	make -f Makefile.pcc

gcc386:
	echo Building 80386+ version of abered with djgpp (gcc for ibm)
	make mlj_edit.o
	make -f Makefile.djp

unix:
	echo Building UNIX abered
	make -f Makefile.gcc

	

mlj_edit.o: ..\editor\mlj_edit.c ..\editor\mlj_edit.h
	chdir ..\editor
	gcc -O3 -Wall -c mlj_edit.c
	copy mlj_edit.o ..\src
	copy mlj_edit.h ..\src
