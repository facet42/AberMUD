#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

/*
 *	AberMUD boot up program. 
 *
 *	1.00	AGC	Prehistory	Original boot up program
 *	1.01	AGC	27/02/93	Passes arguments on
 *
 */

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

int main(int argc,char *argv[])
{
#ifndef _WIN32
	long t,t2;
#else
	time_t t, t2;
#endif

	int pid = 0;
	printf("Aberystwyth Multi-User Dungeon (Revision 5.21 BETA 5)\n\
(c) Copyright 1987-1993, Alan Cox\n\
\n");

#ifndef _WIN32
	close(0);
	close(1);
	ioctl(2,TIOCNOTTY,0);
	close(2);
	setpgrp();
	if (fork() != 0)
		exit(1);
	open("server_log", O_WRONLY | O_CREAT | O_APPEND, 0600);
	dup(0);
	dup(0);
#else
	_open("server_log", O_WRONLY | O_CREAT | O_APPEND, 0600);
#endif

	while(1)
	{
		time(&t);
		argv[0]="AberMUD 5.21 Beta1";

#ifndef _WIN32
		pid=fork();
#endif

		// Spawned as child process
		if(pid==0)
		{
			execvp("./server",&argv[0]);
			perror("./server");
			exit(1);
		}
#ifndef _WIN32
		else
			if(pid!= -1)
			{
				// Waits for a change in the status of the child process
				wait(NULL);
			}
#endif

		time(&t2);
		if(t2-t<10)
		{
			printf("Spawning too fast - error ??\n");
			exit(1);
		}
	}
}
