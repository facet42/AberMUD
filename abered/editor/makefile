# Make file for building test program for mlj_edit/edit string
#
# J.Willie 

test.exe: test
	coff2exe test

test: test.o mlj_edit.o
	gcc -o test mlj_edit.o test.o -lpc

mlj_edit.o: mlj_edit.c
	gcc -Wall -ansi -c mlj_edit.c

test.o: test.c
	gcc -Wall -c test.c
