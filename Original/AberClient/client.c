#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <termio.h>
#include <sys/types.h>
#include <sys/time.h>

extern int Make_Connection(int, const char *);

static void Shutdown(void)
{
	endwin();
	exit(1);
}

static void Error(char *x)
{
	endwin();
	printf("%s\n",x);
	exit(1);
}

static void CharInsert(char *x, int o)
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

static void WaitFD(int fd)
{
	fd_set rdw;
	static struct timeval tlim={1,0};
	FD_ZERO(&rdw);
	FD_SET(fd,&rdw);
	if(select(32,&rdw,NULL,&rdw,&tlim)==-1)
		Error("Select Failed");
}

static WINDOW *top,*bottom;
static int TopSize,BottomSize,Width;


static void SetUp(void)
{
	initscr();
	Width=COLS;
	TopSize=LINES-8;
	BottomSize=7;
	top=newwin(TopSize,Width,0,0);
	bottom=newwin(BottomSize,Width,TopSize+1,0);
	if(top==NULL || bottom==NULL)
		Error("Window create failed.\n");
	wmove(top,TopSize-1,0);
}

static char Input[514];
static char YankBuffer[514];
static char BackBuffer[514];
static int InputPos=0;
static char Prompt[256]="By what name shall I call you : ";
static int GoAhead=1;
static int Echo=1;
static int State=0;
static char Buffer[514];
static int BufPtr=0;

static void CursorCalc(int pos, int *x, int *y)
{
	pos+=strlen(Prompt);
	*x=pos%Width;
	*y=pos/Width;
}

static void DoEvent(void)
{
	Buffer[BufPtr]=0;
	BufPtr=0;
/*	wprintw(top, "EVENT %s.\n", Buffer); */
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

int main(int argc, char *argv[])
{
	int fd;
	int txo;
	int x,y;
	const char *host="127.0.0.1";
	int port=5001;
	if(argc>3)
	{
		fprintf(stderr,"%s <host> <port>\n",argv[0]);
		exit(1);
	}
	if(argc==3)
		port=atoi(argv[2]);
	if(argc>1)
		host=argv[1];
	fd=Make_Connection(port,host);
	if(fd==-1)
	{
		perror(argv[1]);
		exit(1);
	}
	SetUp();
	refresh();
	scrollok(top,TRUE);
	idlok(top,TRUE);
	idlok(bottom,TRUE);
	keypad(stdscr,TRUE);
	halfdelay(1);
	noecho();
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
							wmove(top,TopSize-1,0);
							wclrtoeol(top);
							wmove(top,TopSize-1,0);
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
		if(l < 0 && errno!=EWOULDBLOCK)
			Error("Connection lost\n");
		if(l == 0)
			Error("Remote closed connection\n");
			
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
		l=getch();
		if(l!=ERR)
		{
			unsigned char c=l;
			if(l==8||l==127||l==KEY_BACKSPACE)
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
				beep();
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
}
