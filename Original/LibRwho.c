/*
 *	This is Marcus J Ranum's RWHO support library file. It is part of the mudwho distribution from decuac.dec.com
 *	and is not part of AberMUD5, but is used in compiling it. The AberMUD license has no bearing on this file, 
 *	instead consult the license and information with the mudwho package. If you intend to set up a mudwho server
 *	you will need this package anyway.
 *
 *	Alan
 */


/*
	Copyright (C) 1991, Marcus J. Ranum. All rights reserved.
	
	AmiTCP port Alan Cox 1993.

static	char RCSid[] unused = "$Header: /usr/users/mjr/hacks/umud/RWHO/RCS/clilib.c,v 1.2 91/08/18 21:49:44 mjr Exp $";

*/

/*
code to interface client MUDs with rwho server

this is a standalone library.
*/

#ifdef AMIGA
#include	<exec/types.h>
typedef long	time_t;
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<time.h>
#include	<unistd.h>
#ifdef VMS
#include        "tintop_dec:[amolitor.foo]types.h"
#include        "tintop_dec:[amolitor.foo]socket.h"
#include        "tintop_dec:[amolitor.foo]in.h"
#include        "tintop_dec:[amolitor.foo]netdb.h"
#else
#include	<fcntl.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#endif
#ifdef AMIGA
#include 	<pragmas/socket_pragmas.h>
extern long 	SocketBase;
#endif

#define	DGRAMPORT		6888

#ifndef	NO_HUGE_RESOLVER_CODE
extern	struct	hostent	*gethostbyname();
#endif

static	int	dgramfd = -1;
static	char	*password;
static	char	*localnam;
static	char	*lcomment;
static	struct	sockaddr_in	addr;
static  time_t	senttime;


/* enable RWHO and send the server a "we are up" message */
int rwhocli_setup(char *server, char *serverpw, char *myname, char *comment)
{
#ifndef	NO_HUGE_RESOLVER_CODE
	struct	hostent		*hp;
#endif
	char			pbuf[512];
	char			*p;

	if(dgramfd != -1)
		return(1);

	password = malloc(strlen(serverpw) + 1);
	localnam = malloc(strlen(myname) + 1);
	lcomment = malloc(strlen(comment) + 1);
	if(password == (char *)0 || localnam == (char *)0 || lcomment == (char *)0)
		return(1);
	strcpy(password,serverpw);
	strcpy(localnam,myname);
	strcpy(lcomment,comment);

	p = server;
	while(*p != '\0' && (*p == '.' || isdigit(*p)))
		p++;

	if(*p != '\0') {
#ifndef	NO_HUGE_RESOLVER_CODE
		if((hp = gethostbyname(server)) == (struct hostent *)0)
			return(1);
		(void)bcopy(hp->h_addr,(char *)&addr.sin_addr,hp->h_length);
#else
		return(1);
#endif
	} else {
		unsigned long	f;

		if((f = inet_addr(server)) == -1L)
			return(1);
		(void)bcopy((char *)&f,(char *)&addr.sin_addr,sizeof(f));
	}

	addr.sin_port = htons(DGRAMPORT);
	addr.sin_family = AF_INET;

	if((dgramfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		return(1);

	time(&senttime);

	sprintf(pbuf,"U\t%.20s\t%.20s\t%.20s\t%.10ld\t0\t%.25s",
		localnam,password,localnam,(long)senttime,comment);
	sendto(dgramfd,pbuf,strlen(pbuf),0,(struct sockaddr *)&addr,sizeof(addr));
	return(0);
}





/* disable RWHO */
int rwhocli_shutdown(void)
{
	char	pbuf[512];

	if(dgramfd != -1) {
		sprintf(pbuf,"D\t%.20s\t%.20s\t%.20s",localnam,password,localnam);
		sendto(dgramfd,pbuf,strlen(pbuf),0,(struct sockaddr *)&addr,sizeof(addr));
		close(dgramfd);
		dgramfd = -1;
		free(password);
		free(localnam);
	}
	return(0);
}





/* send an update ping that we're alive */
int rwhocli_pingalive(void)
{
	char	pbuf[512];

	if(dgramfd != -1) {
		sprintf(pbuf,"M\t%.20s\t%.20s\t%.20s\t%.10ld\t0\t%.25s",
			localnam,password,localnam,(long)senttime,lcomment);
		sendto(dgramfd,pbuf,strlen(pbuf),0,(struct sockaddr *)&addr,sizeof(addr));
	}
	return(0);
}





/* send a "so-and-so-logged in" message */
int rwhocli_userlogin(char *uid, char *name, time_t tim)
{
	char	pbuf[512];

	if(dgramfd != -1) {
		sprintf(pbuf,"A\t%.20s\t%.20s\t%.20s\t%.20s\t%.10ld\t0\t%.20s",
			localnam,password,localnam,uid,(long)tim,name);
		sendto(dgramfd,pbuf,strlen(pbuf),0,(struct sockaddr *)&addr,sizeof(addr));
	}
	return(0);
}





/* send a "so-and-so-logged out" message */
int rwhocli_userlogout(char	*uid)
{
	char	pbuf[512];

	if(dgramfd != -1) {
		sprintf(pbuf,"Z\t%.20s\t%.20s\t%.20s\t%.20s",
			localnam,password,localnam,uid);
		sendto(dgramfd,pbuf,strlen(pbuf),0,(struct sockaddr *)&addr,sizeof(addr));
	}
	return(0);
}

