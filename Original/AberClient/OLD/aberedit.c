/*
 *	DOS Type Front End Handler
 *
 *	Release 5.14	AMIGA & Unix
 *
 *	Keyboard handler based on a routine by Whalemeat
 */

#include <stdio.h>
#include <signal.h>
#ifdef AMIGA
#include <stdlib.h>
#endif
#include <string.h>
#ifdef AMIGA
#include "/communications.h"
#include "/IPC.h"
#else
#include "../communications.h"
#include "../IPC.h"
#endif
#include "Keydef.h"

#define PORT_PATH "MYTHOS"
#define FRONT_CHDIR "MUD:"

char  SysPrompt[128]="By what name shall I call you : ";
short SysEcho=1;
short SysSignal=-1;	/* Unset */
short SysMyCode=-1;	/* Unset */
short SysReadOk=0;	/* Read disabled */
short SysRedraw=0;	/* Redraw control */
PORT *SysMPort=NULL;		
short SysEditing=0;
char SysEditLine[512];	/* Edit Buffer - For Download And Edit */
PORT *DaemonPort=NULL;
short Con_Reading=0;
short Con_Echo=0;
char Con_Prompt[80]="";
short Con_Dirty=0;
short ComInit=0;
char LineBlock[80];
short LinePointer=0;

#define TTY_COOKED 0		/* Terminal Defines */
#define TTY_RAW	   1

extern void initln(),endln();
extern char *getln();

/*
 *	machine dependant tty stuff
 */

#ifdef AMIGA

void SetTerminal(m)
int m;
{
	SetConsoleMode(m);
}

#else

/* Unix version */

void SetTerminal(m)
int m;
{
	if(m==TTY_COOKED)
	{
		RestoreTTY();
	}
	else
	{
		SetTTY();
	}
}

#endif

void HangUp()
{
	extern void WrapUp();
#ifdef SIGHUP
	signal(SIGHUP,SIG_DFL);
#endif
	SetTerminal(TTY_COOKED);
	if(SysMPort!=NULL)
	{
		SendTPacket(PACKET_CLEAR,"hangup");
	}
	if(SysMPort)
	{
		CloseMPort(SysMPort);	/* Dont need delete - temp port anyway*/
	}
	if(DaemonPort)
	{
		CloseMPort(DaemonPort);
	}
	exit(0);
}

void PipeUp()
{
	fprintf(stderr,"Broken pipe - call a plumber!\n");
	WrapUp();
}
	
void WrapUp()			/* Called whenever we exit() */
{
	endln();
	SetTerminal(TTY_COOKED);
	if(SysMPort)
	{
		CloseMPort(SysMPort);	/* Dont need delete - temp port anyway*/
	}
	if(DaemonPort)
	{
		CloseMPort(DaemonPort);
	}
	exit(0);
}

void Show_File(x)
char *x;
{
	char buf[256];
	FILE *f=fopen(x,"r");
	if(f!=NULL)
	{
		while(fgets(buf,255,f)!=NULL)
			fputs(buf,stdout);
		fclose(f);
	}
}

void SetUpDisplay()
{
#ifdef AMIGA
	printf("\032\012\032");	/* USE TERMCAP cl */
#endif
	initln();
}


Con_Write(x)
char *x;
{
	write(fileno(stdout),x,strlen(x));
	return(0);
}

Con_WriteChar(c)
char c;
{
	write(fileno(stdout),&c,1);
	return(0);
}

char readph()
{
	int c;
wt:	c=WaitEvent();
	if(c!=-1)
		return((char)c);
	if(Con_Dirty)
	{
		Con_Dirty=0;
		return((char)tty_what);
	}
	goto wt;
}

void Con_Redraw()
{
	if(!Con_Reading)
		return;
	Con_Dirty=1;
}

#ifndef AMIGA

int dostopped=0;

int DoStopped()
{
	dostopped=1;
	return(1);
}

#endif 

#ifdef UNIX
BleepBleep()
{
	printf("Timeout....\n");
	signal(SIGALRM,WrapUp);
	alarm(5);	/* Allow 5 seconds for the I've died attempt */
	HangUp();	/* Try and report a flop */
}
#endif

int WaitEvent()
{
	static int TimeOut=30;
	static int WaitTime=15;	/* Short login timeout */
	char c=-1;
	int s=SysReadOk;
	SysRedraw=0;
#ifdef UNIX
	if(s==0)
	{
		signal(SIGALRM,BleepBleep);
		alarm(WaitTime);
		WaitTime=60;	/* Afterwards use log timeouts */
		BlockOff(SysMPort);
		BlockOff(DaemonPort);
	}
	else
	{
		BlockOn(SysMPort);
		alarm(0);
	}
#endif
	ProcessPackets();	/* Handle everything which comes in */
	if(SysRedraw) Con_Redraw();	/* Check input lines */
#ifndef AMIGA
	if(s!=SysReadOk)
		return(-1);
#endif
#ifdef AMIGA
	if(s==0)
		Delay(10);
	else
		if(WaitForChar(Input(),200000)==-1)
			read(fileno(stdin),&c,1);
#else
	if(s==0)
	{
		if(TimeOut++>60)
		{
			printf("OVERRUN Timeout..\n");
			HangUp();
			exit(1);
		}
		return(-1);
	}
	else
	{
		TimeOut=0;
		c=getchar();	/* Will be interrupted by timer */
		alarm(0);	/* Alarm Off */
/*		if(dostopped)	/* Set by signal function */
/*			return(-1);*/
		return((int)c);
	}
#endif
	return((int)c);
}

#define prevbuf(x)	(((x)-bufhalf)&bufsiz)
#define nextbuf(x)	(((x)+bufhalf)&bufsiz)

char *backspc,*spc,*buffer;

void initln()
{
	char *s,*bs=(char *)malloc(bufhalf*2+bufsize);
	int i;
	buffer=(spc=s=(backspc=bs)+bufhalf)+bufhalf;
	if(s==0)
	{
		printf("Out Of Memory\n");
		exit(8);
	}
	for(i=0;i++<bufmax;*s++=' ',*bs++='\010');
}

void endln()
{
	if(backspc)
	{
		free(backspc);
		backspc=NULL;	/* Fixed AGC */
	}
}


short C_Posn=0;	/* Used to track for bkspc -> cursor up */

char *getlin(msg,dest,echo,length)
char *msg,*dest;
int echo,length;
{
	char *line,*lin2;
	if(echo==EDITLN&&dest)
	{
/*		printf("EDITING\n");	*/
		getln(dest,LODSTR,length);
	}
	lin2=line=getln(msg,echo,length);
	if(dest)
		while(*dest++=*lin2++);
	return(line);
}

char *shift(buftop,bufend,from,cp,ep,echo)
char *buftop,*bufend,*from,*cp,*ep;
{
	char *tp=buftop;
	while(*tp++=*from++)
		if(tp==bufend)
			break;
	*tp--=0;
	if(echo)
	{
		if(cp>buftop) writeph(backspc,cp-buftop);
		if(tp>buftop) writeph(buftop,tp-buftop);
		if(tp<ep)
		{
			writeph(spc,ep-tp);
			writeph(backspc,ep-tp);
		}
	}
	return(tp);
}

void say(str)
char *str;
{
	if(str)
		writeph(str,strlen(str));
	writeph("\n",1);
}

void prompt(str)
char *str;
{
	if(str)
		writeph(str,strlen(str));
	C_Posn=strlen(str);
}

char *getln(msg,echo,num_char)
char *msg;
int echo,num_char;
{
	static int half;
	char *buftop,*bufend,*cp,*ep,*tp,*tp2,c,cbuf;
	int bufptr=half;
	if(num_char>bufmax||num_char<0) num_char=bufmax-1;
	bufend=(ep=cp=buftop=buffer+(half=nextbuf(half)))+num_char;
	if(echo==LODSTR)
	{
		while(*ep++=*msg++)
			if(ep==bufend)
				break;
		*ep=0;
		return(cp);
	}
	strcpy(Con_Prompt,msg);
	prompt(msg);
	if(echo==EDITLN)
		cp=ep=shift(buftop,bufend,buffer+prevbuf(half),cp,
			    ep,(echo=ECHO));
	for(;;)
	{
		c=readph(&cbuf,1);
		c&=0x7F;
		if(c=='\r'||c=='\n')
		{
			say(NULL);
			break;
		}
		else if(c>=' ' && c<=126)
		{
			if(ep<bufend)
			{
				tp2=tp=ep++;
				while(tp2-->cp) *tp--=*tp2;
				*cp++=c;
				if(echo)
				{
					writeph(tp,ep-tp);
					if(ep>cp)
						writeph(backspc,ep-cp);
				}
			}
			else
				prompt("\007");
		}
		else if(c==tty_del||c==tty_bs)
		{
			if(cp!=buftop)
			{
				cp--;
				if(echo)
				{
					*cp='\010';
					*ep=' ';
					writeph(cp,ep-cp+1);
					writeph(backspc,ep-cp);
				}
				tp2=(tp=cp)+1;
				*ep--=0;
				while(*tp++=*tp2++);
			}
		}
		else if(c==tty_delr)
		{
			if(cp!=ep)
			{
				if(echo)
				{
					*ep=' ';
					writeph(cp+1,ep-cp);
					writeph(backspc,ep-cp);
				}
				tp2=(tp=cp)+1;
				*ep--=0;
				while(*tp++=*tp2++);
			}
		}
		else if(c==tty_tab)
		{
			int z=5-((cp-buftop)%5);
			if(ep+z<bufend)
			{
				ep=(tp=(tp2=ep)-1+z)+1;
				while(tp2--<cp)
					*tp--=*tp2;
				while(tp>=cp)
					*tp--=' ';
				writeph(cp,ep-cp);
				cp+=z;
				writeph(backspc,ep-cp);
			}
			else
				prompt("\007");
		}
		else if(c==tty_left||c==tty_back)
		{
			if(cp!=buftop)
			{
				cp--;
				if(echo)
					writeph(backspc,1);
			}
		}
		else if(c==tty_fwrd)
		{
			if(cp!=ep)
			{
				if(echo)
					writeph(cp,1);
				cp++;
			}
		}
		else if(c==tty_top||c==tty_home)
		{
			if(cp!=buftop)
			{
				if(echo) writeph(backspc,cp-buftop);
				cp=buftop;
			}
		}
		else if(c==tty_end)
		{
			if(cp!=ep)
			{
				if(echo) writeph(cp,ep-cp);
				cp=ep;
			}
		}
		else if(c==tty_rept)
			cp=ep=shift(buftop,bufend,buffer+(bufptr=prevbuf(half)),
				    cp,ep,echo);
		else if(c==tty_prev)
		{
			if((bufptr=prevbuf(bufptr))==half)
				bufptr=prevbuf(bufptr);
			cp=ep=shift(buftop,bufend,buffer+bufptr,cp,ep,echo);
		}
		else if(c==tty_last)
		{
			readph(&c,1);
			c&=0x7f;
			if(c>'0'&&c<'4')
				cp=ep=shift(buftop,bufend,
			buffer+(bufptr=(abs(half-(c-'0')*bufhalf))&bufsiz),
					    cp,ep,echo);
		}
		else if(c==tty_kill)
		{
			if(echo)
			{
				if(cp>buftop)
					writeph(backspc,cp-buftop);
				if(ep>buftop)
				{
					writeph(spc,ep-buftop);
					writeph(backspc,ep-buftop);
				}
			}
			ep=cp=buftop;
		}
		else if(c==tty_what)
		{
			writeph("\r",1);
			prompt(msg);
			if(echo&&ep>buftop)
			{
				writeph(buftop,ep-buftop);
				if(cp<ep)
					writeph(backspc,ep-cp);
			}
		}
		else if(c==tty_quit||c==tty_qut2)
		{
			*bufend='^';
			bufend[1]=c|0x40;
			bufend[2]='\n';
			bufend[3]='\r';
			if(echo&&ep>cp+2)
			{
				writeph(spc,ep-cp);
				writeph(backspc,ep-cp);
			}
			writeph(bufend,4);
			if(c==tty_quit)
				say("*CANCEL*");
			else
				say("*INTERRUPT*");
			prompt(msg);
			if(echo)
			{
				if(ep>buftop)
					writeph(buftop,ep-buftop);
				if(cp<ep)
					writeph(backspc,ep-cp);
			}
		}	
	}
	*ep=0;
	if(echo==NOECHO||ep==buftop) half=prevbuf(half);
	return(buftop);
}

static char OutBuf[128];
static short OutPtr=0;

void bflush()
{
	OutBuf[OutPtr]=0;
	printf("%s",OutBuf);
	OutPtr=0;
}

void bprintc(c)
char c;
{
	OutBuf[OutPtr++]=c;
	if(OutPtr==127)
		bflush();
}

void bprints(s)
unsigned char *s;
{
	while(*s)
		bprintc(*s++);
}

int writeph(string,l)
char *string;
int l;
{
	char c=string[l];
	char *x;
	if(l<1) return(0);
	string[l]=0;
	x=string;
	while(*x)
	{
		if(*x==8)
			C_Posn--;
		if(*x==13||*x==10)
			C_Posn=0;
		if(*x>31)
			C_Posn++;
/*		if(*x==8&&C_Posn==0)
			bprints("\033[\x46\033[76\x48");
		else*/
			bprintc(*x);
		x++;
		if(C_Posn==76)
			C_Posn=0;	/* Set to tty width */
	}
/*	printf("%s",string);	*/
	string[l]=c;
	bflush();
	fflush(stdout);
	return(0);
}

Con_Read(prompt,buffer,length,echoflag)
char *prompt;
char *buffer;
int length;
int echoflag;
{
	int e=NOECHO;
	if(echoflag)
		e=ECHO;
	Con_Echo=echoflag;
	strcpy(Con_Prompt,prompt);
	Con_Reading=1;
	if(SysEditing)
	{
		strcpy(buffer,SysEditLine);
		getlin(Con_Prompt,buffer,EDITLN,490);
	}
	else
		getlin(Con_Prompt,buffer,e,490);
	SysEditing=0;
	Con_Reading=0;
	return(0);
}


char TermBuffer[1024];
char PC=0;
short ospeed=0;
short ttywidth=76;
		
void main(argc,argv)
int argc;
char *argv[];
{
	char buffer[512];
	char *t;
	signal(SIGINT,SIG_IGN);
#ifdef SIGTSTP
	signal(SIGTSTP,SIG_IGN);
#endif
#ifdef SIGQUIT
	signal(SIGQUIT,SIG_IGN);
#endif
#ifdef SIGHUP
	signal(SIGHUP,HangUp);
#endif
#ifdef SIGPIPE
	signal(SIGPIPE,PipeUp);
#endif
#ifdef UNIX
	signal(SIGSEGV,HangUp);
	signal(SIGBUS,HangUp);
	signal(SIGILL,HangUp);
	signal(SIGFPE,HangUp);
	signal(SIGSYS,HangUp);
#endif
	if(chdir(FRONT_CHDIR)==-1)
	{
		fprintf(stderr,"AberMUD seems to be concealed...\n");
		exit(1);
	}
#ifdef UNIX
	if(!isatty(fileno(stdin)))
#else
	if(!IsInteractive(Input()))
#endif
	{
		fprintf(stderr,"There seems to be a terminal redirection problem here.\n");
		exit(1);
	}
	printf("\033[0;0H\033[J");
	fflush(stdout);
	Show_File("motd");
	SetUpDisplay();
	FindDaemon();
	Con_Write("[WAIT]\r");
	HandleLogin();
	SetTerminal(TTY_RAW);
	while(1)
	{
		while(SysReadOk==0)
		{
			WaitEvent();
		}
		Con_Read(SysPrompt,buffer,70,1-SysEcho);
		SendTPacket(PACKET_COMMAND,buffer);
		SysReadOk=0;	/* Disabled automatically by system */
	}	
}

	
InterpretPacket(x)
COMDATA *x;
{
	COMTEXT *y=(COMTEXT *)x;
	switch(x->pa_Type)
	{
		case PACKET_CLEAR:
			
			DoOutput(y->pa_Data);
			DoOutput("\n");
			SendNPacket(PACKET_CLEARED,0,0,0,0);
			WrapUp();
			break;

		case PACKET_LOOPECHO:

			SendNPacket(PACKET_ECHOBACK,0,0,0,0);
			break;

		case PACKET_ECHOBACK:
		
			break;

		case PACKET_OUTPUT:

			DoOutput(y->pa_Data);
			break;

		case PACKET_SETPROMPT:

			strcpy(SysPrompt,y->pa_Data);
			break;

		case PACKET_ECHO:

			SysEcho=x->pa_Data[0];
			break;

		case PACKET_SNOOPTEXT:

			SnoopDriver(y->pa_Data);
			break;

		case PACKET_INPUT:
			
			SysReadOk=x->pa_Data[0];
			break;

		case PACKET_LOGINACCEPT:
		
			SysMyCode=x->pa_Data[1];
			Con_Write("                  \n");
			break;

		case PACKET_EDIT:
			
			SysEditing=1;
			strcpy(SysEditLine,y->pa_Data);
			break;

		case PACKET_SETTITLE:

			break;

		case PACKET_PICTURE:

			break;

		case PACKET_SETFIELD:

			SetField(x->pa_Data[0]);
			break;
	}
	return(0);
}
	



HandleLogin()
{
	char buffer[128];
	SysMPort=CreateMPort(FL_TEMPORARY);
	if(SysMPort==NULL)
	{
		WrapUp();
	}
#ifdef AMIGA
	SysSignal=SysMPort->po_Signal;
#endif
#ifdef AMIGA
	sprintf(buffer,"Someone$%ld",SysMPort);
#else
	sprintf(buffer,"%d$%ld",getuid(),getpid());
#endif
	SendTPacket(PACKET_LOGINREQUEST,buffer);
	return(0);
}

	
SendBlock(block,size)
char *block;
int size;
{
	int er;
#ifndef AMIGA
	BlockOff(DaemonPort);
#endif
	if((er=WriteMPort(DaemonPort,block,size))<0)
	{
		fprintf(stderr,"Port write error :%d\n",er);
		WrapUp();
	}
	return(1);
}
		
SendTPacket(class,data)
short class;
char *data;
{
	COMTEXT a;
	a.pa_Type=class;
	a.pa_Sender=SysMyCode;
	strcpy(a.pa_Data,data);
	return(SendBlock(&a,sizeof(COMTEXT)));
}
	
SendNPacket(class,p1,p2,p3,p4)
short class;
short p1,p2,p3,p4;
{
	COMDATA a;
	a.pa_Type=class;
	a.pa_Sender=SysMyCode;
	a.pa_Data[0]=p1;
	a.pa_Data[1]=p2;
	a.pa_Data[2]=p3;
	a.pa_Data[3]=p4;
	return(SendBlock(&a,sizeof(COMDATA)));
}


static short wptr;

SetField(x)
int x;
{
	while((LinePointer+wptr)%x)
		OutChar(' ');
}

OutChar(x)
char x;
{
	static char WordBuffer[81];
	if((x=='\n')||(x==' ')||(wptr>76))
	{
		WordBuffer[wptr++]=x;
		if(wptr+LinePointer>77)
		{
			LineBlock[LinePointer]=0;
			BlockDoOutput(LineBlock);
			BlockDoOutput("\n");
			LinePointer=0;
		}
		strncpy(LineBlock+LinePointer,WordBuffer,wptr);
		LinePointer+=wptr;
		wptr=0;
	}
	if(x=='\n')
	{
		LineBlock[LinePointer]=0;
		BlockDoOutput(LineBlock);
		LinePointer=0;
		return;
	}
	if(x!=' ')
		WordBuffer[wptr++]=x;
}
	
DoOutput(x)
char *x;
{
	while(*x) OutChar(*x++);
	return(0);
}

BlockDoOutput(x)
char *x;
{
	if((Con_Reading)&&(SysRedraw==0))
	{
		SysRedraw=1;
		Con_Write("\r\
                                                                      \r");
	}
	Con_Write(x);
	return(0);
}

GetPacket(port,packet)
PORT *port;
COMTEXT *packet;
{
	int er;
#ifndef AMIGA
	if(SysReadOk==0)
		BlockOff(port);
	else
		BlockOn(port);
#endif
	if((er=ReadMPort(port,packet))<0)
	{
		if(er==-3||er==-2)
			return(-2);
		fprintf(stderr,"Port read error: %d\n",er);
		WrapUp();
	}
	return(0);
}

ProcessPackets()
{
	int er;
	COMTEXT p;
	while(1)
	{
		er=GetPacket(SysMPort,&p);
		if(er==-2)
			break;
		InterpretPacket((COMDATA *)&p);
	}
	return(0);
}


FindDaemon()
{
	DaemonPort=FindService(PORT_PATH);
	if(DaemonPort)
		DaemonPort=OpenMPort(DaemonPort);	/* Open it */
	if(DaemonPort==NULL)
	{
		fprintf(stderr,"Sorry, AberMUD V is not currently running.\n");
		WrapUp();
	}
	return(0);
}



void OutSnoop(x)
char x;
{
	static char SnoopBuffer[128];
	static char WordBuffer[81];
	static short wptr,SnoopPointer;
	if((x=='\n')||(x==' ')||(wptr>76))
	{
		WordBuffer[wptr++]=x;
		if(wptr+SnoopPointer>77)
		{
			SnoopBuffer[SnoopPointer]=0;
			BlockDoOutput("|");
			BlockDoOutput(SnoopBuffer);
			BlockDoOutput("\n");
			SnoopPointer=0;
		}
		strncpy(SnoopBuffer+SnoopPointer,WordBuffer,wptr);
		SnoopPointer+=wptr;
		wptr=0;
	}
	if(x=='\n')
	{
		SnoopBuffer[SnoopPointer]=0;
		BlockDoOutput("|");
		BlockDoOutput(SnoopBuffer);
		SnoopPointer=0;
		return;
	}
	if(x!=' ')
		WordBuffer[wptr++]=x;
}

SnoopDriver(x)
char *x;
{
	while(*x)
		OutSnoop(*x++);
}
	
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    