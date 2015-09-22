CFILES  = client.c socket.c

OFILES  = client.o socket.o


LDFLAGS = -lcurses
ECHO    = /bin/echo
MV	= /bin/mv
TOUCH   = touch

CC	= gcc -Wall -pedantic

all     : client
	@${ECHO}   AberMUD5 client is up to date.

client  : ${OFILES}
	${CC} ${LDFLAGS} ${OFILES}
	@${TOUCH} client
	@${MV} client client.old
	@${MV} a.out client

clean:
	rm -f *.o *~ client client.old


