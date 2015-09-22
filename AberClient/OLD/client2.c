#include <curses.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

void CharInsert(x,o)
char *x;
int o;
{
	int ct;
	x+=o;
	ct=strlen(x);
	while(ct>=0)
	{
		x[ct+1]=x[ct];
		ct--;
	}
}

void WaitFD(fd)
int fd;
{
	int rdw=(1<<fd);
	static struct timeval tlim={1,0};
	if(select(32,&rdw,NULL,&rdw,&tlim)==-1)
		Error("Select Failed");
}

WINDOW *top,*bottom;
int TopSize,BottomSize,Width;

Shutdown()
{
	endwin();
#ifndef NEW_CURSES
	KeyRestore();
#endif
	exit(1);
}

Error(x)
char *x;
{
	endwin();
	printf("%s\n",x);
#ifndef NEW_CURSES
	KeyRestore();
#endif
	exit(1);
}

SetUp()
{
	initscr();
	signal(SIGINT,Shutdown);
	Width=COLS;
	TopSize=LINES-7;
	BottomSize=7;
	top=newwin(TopSize,Width,0,0);
	bottom=newwin(BottomSize,Width,TopSize+1,0);
	if(top==NULL || bottom==NULL)
		Error("Window create failed.\n");
	wmove(top,TopSize,0);
}

char Input[514];
char YankBuffer[514];
char BackBuffer[514];
int InputPos=0;
char Prompt[256]="By what name shall I call you : ";
int GoAhead=1;
int Echo=1;
int State=0;
char Buffer[514];
int BufPtr=0;

void CursorCalc(pos,x,y)
int pos;
int *x,*y;
{
	pos+=strlen(Prompt);
	*x=pos%Width;
	*y=pos/Width;
}

DoEvent()
{
	Buffer[BufPtr]=0;
	BufPtr=0;
	if(*Buffer=='Q')
		Shutdown();
	if(*Buffer=='P')
	{
		strcpy(Prompt,Buffer+1);
		return;
	}
	if(*Buffer=='T')
	{
		strcpy(Input,Buffer+1);
		InputPos=strlen(Input);
		return;
	}
	if(*Buffer=='G')
	{
		werase(bottom);
/*		wprintw(top,"[GAH]\n");*/
		wrefresh(top);
		wprintw(bottom,"%s",Prompt);
		wmove(bottom,0,strlen(Prompt));
		wprintw(bottom,"%s",Input);
		wrefresh(bottom);
		GoAhead=1;
		return;
	}
	if(*Buffer=='S')
	{
		wprintw(top,"|%s",Buffer+1);
		return;
	}
	if(*Buffer=='E')
	{
		if(Buffer[1]=='Y')
			Echo=0;
		else
			Echo=1;
		return;
	}
}

#ifndef NEW_CURSES

struct termios TermDef;
int tty_set=0;

void KeyRestore()
{
	if(tty_set)
	{
		ioctl(0,TCSETS,&TermDef);
	}
}

void SetNoEcho()
{
	struct termios temp;
	if(tty_set)
		return;
	if(ioctl(0,TCGETS,&TermDef)==-1)
		return;
	temp=TermDef;
	temp.c_lflag&=~(ICANON|ECHO);
	temp.c_cc[VMIN]=0;
	temp.c_cc[VTIME]=1;
	tty_set=1;
	ioctl(0,TCSETS,&temp);
}

#endif

main(argc,argv)
int argc;
char *argv[];
{
	int fd;
	int txo;
	int x,y;
	if(argc!=3)
	{
		fprintf(stderr,"%s <host> <port>\n",argv[0]);
		exit(1);
	}
	fd=Make_Connection(atoi(argv[2]),argv[1]);
	if(fd==-1)
	{
		perror(argv[1]);
		exit(1);
	}
	SetUp();
	refresh();
	wprintw(top,"AberMUD 5.21BETA3, Client  Alan Cox 1991\n");
	scrollok(top,TRUE);
	idlok(top,TRUE);
	idlok(bottom,TRUE);
#ifdef NEW_CURSES
	keypad(stdscr,TRUE);
	halfdelay(1);
#endif
#ifdef NEW_CURSES
	noecho();
#else
	SetNoEcho();
#endif
	wrefresh(top);
	fcntl(fd,F_SETFL,O_NDELAY);
	while(1)
	{
		char buf[256];
		int l;
		txo=0;
		while((l=read(fd,buf,255))>0)
		{
			int ct=0;
			while(ct<l)
			{
				if(State==1&&buf[ct]!='\003')
					Buffer[BufPtr++]=buf[ct];
				else if (State==1)
				{
					State=0;
					DoEvent();
				}
				else
				{
					if(buf[ct]=='\002')
						State=1;
					else
					{
						if(buf[ct]=='\n')
						{
							scroll(top);
							wmove(top,TopSize,0);
							wclrtoeol(top);
							wmove(top,TopSize,0);
						}
						else
							waddch(top,buf[ct]);
/*						wrefresh(top);*/
					}
					txo=1;
				}
				ct++;
			}
		}
		if(l<=0&&errno!=EWOULDBLOCK&&errno!=0)
			Error("Remote Closed Connection\n");
		if(txo!=0&&GoAhead==1)
		{
			wrefresh(top);
			wrefresh(bottom);
			txo=0;
		}
		if(GoAhead==0)
		{
			WaitFD(fd);
			continue;
		}
#ifdef NEW_CURSES
		l=getch();
#else
		{
			char tc;
			if(read(0,&tc,1)!=1)
				l=ERR;
			else
				l=tc;
		}
#endif
		if(l!=ERR)
		{
			char c=l;
			if(l==8||l==127)
			{
				if(InputPos)
				{
					InputPos--;
					strcpy(Input+InputPos,Input+InputPos+1);
					CursorCalc(InputPos,&x,&y);
					wmove(bottom,y,x);
					wprintw(bottom,"%s ",Input+InputPos);
					wmove(bottom,y,x);
					wrefresh(bottom);
				}
				continue;
			}
#ifndef KEY_LEFT
#define KEY_LEFT	2
#define KEY_RIGHT	6
#endif
			if((l==KEY_LEFT||l==2)&&InputPos)
			{
				InputPos--;
				CursorCalc(InputPos,&x,&y);
				wmove(bottom,y,x);
				wrefresh(bottom);
				continue;
			}
			if((l==6||l==KEY_RIGHT)&&InputPos<strlen(Input))
			{
				InputPos++;
				CursorCalc(InputPos,&x,&y);
				wmove(bottom,y,x);
				wrefresh(bottom);
				continue;
			}
			if(c==1)
			{
				InputPos=0;
				CursorCalc(InputPos,&x,&y);
				wmove(bottom,y,x);
				wrefresh(bottom);
				continue;
			}
			if(c==5)
			{
				InputPos=strlen(Input);
				CursorCalc(InputPos,&x,&y);
				wmove(bottom,y,x);
				wrefresh(bottom);
				continue;
			}
			if(c==25)
			{
				werase(bottom);
				strcpy(Input,YankBuffer);
				InputPos=strlen(Input);
				wmove(bottom,0,0);
				wprintw(bottom,"%s",Prompt);
				wmove(bottom,0,strlen(Prompt));
				wprintw(bottom,"%s",Input);
				wrefresh(bottom);
				continue;
			}
			if(c=='R'-64)
			{
				werase(bottom);
				strcpy(Input,BackBuffer);
				InputPos=strlen(Input);
				wmove(bottom,0,0);
				wprintw(bottom,"%s",Prompt);
				wmove(bottom,0,strlen(Prompt));
				wprintw(bottom,"%s",Input);
				wrefresh(bottom);
				continue;
			}
			if(c=='K'-64)
			{
				strcpy(YankBuffer,Input);
				continue;
			}
			if(c==24)
			{
				InputPos=0;
				*Input=0;
				werase(bottom);
				wmove(bottom,0,0);
				wprintw(bottom,"%s",Prompt);
				wrefresh(bottom);
				continue;
			}
			if(c=='\r'||c=='\n')
			{
				c='\n';
				GoAhead=0;
				if(write(fd,Input,strlen(Input))!=strlen(Input))
					Error("Remote closed connection");
				if(write(fd,&c,1)==-1)
					Error("Remote closed connection");
/*				wprintw(top,"[SNT]\n");*/
				strcpy(BackBuffer,Input);
				InputPos=0;
				*Input=0;
				continue;
			}
			if(InputPos==511||c<32||c>127)
			{
#ifdef NEW_CURSES
				beep();
#endif
				continue;
			}
			CharInsert(Input,InputPos);
			Input[InputPos++]=c;
			if(Echo==1)
			{
				wprintw(bottom,"%s",Input+InputPos-1);
				CursorCalc(InputPos,&x,&y);
				wmove(bottom,y,x);
				wrefresh(bottom);
			}
		}
	}
	Shutdown();
}

