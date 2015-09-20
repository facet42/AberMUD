 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

/*
 *	Socket Creation Functions
 *
 *	1.00	Original Version
 *	1.01	License change
 */
 
#ifdef _WIN32
#pragma warning(disable: 4996)
#include <WinSock2.h>
#include <tchar.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef AMIGA
#include <pragmas/socket_pragmas.h>
#endif

#ifndef _WIN32
extern int errno;
#endif

static struct sockaddr_in myaddress;
static int master_socket;

#ifdef AMIGA
long SocketBase;

int exit_cleanup(void)
{
	if(SocketBase!=NULL)
	{
		CloseLibrary(SocketBase);
		SocketBase=NULL;
	}
}
#endif

#ifdef _WIN32
LPTSTR GetErrorString(DWORD error)
{
	LPTSTR buffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, NULL);

	return buffer;
}
#endif

/*
 *	Create the server socket
 */
 
int Make_Socket(int port)
{
#ifdef PARANOIA
	char buf[64];
	struct hostent *host;
#endif	
	int v;
	int tmp = 1;
#ifdef AMIGA
	if (SocketBase == NULL)
	{
		SocketBase = OpenLibrary("bsdsocket.library", 0);
		if (SocketBase == NULL)
		{
			fprintf(stderr, "Fatal error: AmiTCP is not running.\n");
			exit(1);
		}
		SetErrnoPtr(&errno, sizeof(errno));
	}
#endif
#ifdef _WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		perror("Failed to start WinSock\n");
		exit(1);
	}
#endif
#ifdef PARANOIA
	gethostname(buf, 63);
	host = gethostbyname(buf);
	if (host == NULL)
	{
		fprintf(stderr, "INET: Error can't find '%s'\n", buf);
		exit(1);
	}
	myaddress.sin_family = host->h_addrtype;
#endif
	myaddress.sin_family = AF_INET;
	myaddress.sin_port = htons((u_short)port);
	v = (int)socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
	if (v == INVALID_SOCKET)
#else
	if (v == -1)
#endif
	{
		fprintf(stderr, "(%d)", errno);
		perror("INET: socket, ");
		exit(1);
	}

	setsockopt(v, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp));
#ifndef _WIN32
	while (bind(v, (struct sockaddr *)&myaddress, sizeof(myaddress)) < 0)
	{
#else
	int result = bind(v, (struct sockaddr *)&myaddress, sizeof(myaddress));
	while (result == SOCKET_ERROR)
	{
		LPTSTR message = GetErrorString(WSAGetLastError());
		_ftprintf(stderr, message);
		LocalFree(message);
#endif
		if (errno != EADDRINUSE)
		{
#ifdef _WIN32
			closesocket(v);
#else
			close(v);
#endif
			fprintf(stderr, "(%d)", errno);
			perror("INET: bind, ");
			exit(1);
		}

		printf("Address in use: Retrying...\n");
#ifndef _WIN32
		sleep(5);
#else
		Sleep(5000);
		result = bind(v, (struct sockaddr *)&myaddress, sizeof(myaddress));
#endif
	}

	if (listen(v, 5) == -1)
	{
		fprintf(stderr, "(%d)", errno);
		perror("INET: listen,");
		exit(1);
	}

	master_socket = v;
	return(v);
}

