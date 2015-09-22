/*
***************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of 
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
* 
* See the file maintain.er for details of the current maintainer.
***************************************************************************
*/
/******************************************************************************/
/* asciixmas                                                                  */
/* December 1989             Larry Bartz           Indianapolis, IN           */
/*                                                                            */
/*                                                                            */
/* I'm dreaming of an ascii character-based monochrome Christmas,             */
/* Just like the one's I used to know!                                        */
/* Via a full duplex communications channel,                                  */
/* At 9600 bits per second,                                                   */
/* Even though it's kinda slow.                                               */
/*                                                                            */
/* I'm dreaming of an ascii character-based monochrome Christmas,             */
/* With ev'ry C program I write!                                              */
/* May your screen be merry and bright!                                       */
/* And may all your Christmases be amber or green,                            */
/* (for reduced eyestrain and improved visibility)!                           */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* IMPLEMENTATION                                                             */
/*                                                                            */
/* Feel free to modify the defined string FROMWHO to reflect you, your        */
/* organization, your site, whatever.                                         */
/*                                                                            */
/* This really looks a lot better if you can turn off your cursor before      */
/* execution. I wanted to do that here but very few termcap entries or        */
/* terminfo definitions have the appropriate string defined. If you know      */
/* the string(s) for the terminal(s) you use or which your site supports,     */
/* you could call asciixmas from within a shell in which you issue the        */
/* string to the terminal. The cursor is distracting but it doesn't really    */
/* ruin the show.                                                             */
/*                                                                            */
/* At our site, we invoke this for our users just after login and the         */
/* determination of terminal type.                                            */
/*                                                                            */
/*                                                                            */
/* PORTABILITY                                                                */
/*                                                                            */
/* I wrote this using only the very simplest curses functions so that it      */
/* might be the most portable. I was personally able to test on five          */
/* different cpu/UNIX combinations.                                           */
/*                                                                            */
/*                                                                            */
/* COMPILE                                                                    */
/*                                                                            */
/* usually this:                                                              */
/*                                                                            */
/* cc -O asciixmas.c -lcurses -o asciixmas -s                                 */
/*                                                                            */
/*                                                                            */
/* Zilog S8000 models 11, 21, 31, etc with ZEUS variant of SYSTEM III         */
/* maybe other SYSTEM III also:                                               */
/*                                                                            */
/* cc asciixmas.c -lcurses -ltermlib -o asciixmas -s                          */
/*                                                                            */
/* as above with optional "peephole optimizer" installed:                     */
/*                                                                            */
/* cc -O asciixmas.c -lcurses -ltermlib -o asciixmas -s                       */
/*                                                                            */
/*                                                                            */
/* Zilog S8000 models 32, 130 with WE32100 chip and SYS V, REL2               */
/* maybe 3B2 also?                                                            */
/*                                                                            */
/* cc -f -O -K sd asciixmas.c -lcurses -o asciixmas -s                        */
/*                                                                            */
/*                                                                            */
/* Pyramid, Sequent, any other "dual universe" types compile and execute      */
/* under either universe. The compile line for the ucb universe (as you       */
/* might expect) is the same as for SYS III UNIX:                             */
/*                                                                            */
/* cc -O asciixmas.c -lcurses -ltermlib -o asciixmas -s                       */
/*                                                                            */
/* The above compile will also hold true for other BSD systems. (I hope)      */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* For the Scrooges out there among you who don't want this thing to loop     */
/* forever (or until the user hits a key), insert this into your compile        */
/* line just after "cc" :                                                     */
/*                                                                            */
/* -DNOLOOP                                                                   */
/*                                                                            */
/* like so:                                                                   */
/*                                                                            */
/* cc -DNOLOOP -O asciixmas.c -lcurses -o asciixmas -s                        */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef PDCDEBUG
char *rcsid_xmas  = "$Id: xmas.c,v 1.1 2001/01/10 08:27:47 mark Exp $";
#endif

#include <curses.h>
#include <signal.h>

#if defined(XCURSES)
	char *XCursesProgramName = "xmas";
#endif

#define FROMWHO "Mark Hessling - (M.Hessling@qut.edu.au)"

int y_pos, x_pos;

WINDOW
       *treescrn, *treescrn2,*treescrn3, *treescrn4,*treescrn5, *treescrn6,
       *treescrn7, *treescrn8,
       *dotdeer0,
       *stardeer0,
       *lildeer0, *lildeer1, *lildeer2, *lildeer3,
       *middeer0, *middeer1, *middeer2, *middeer3,
       *bigdeer0, *bigdeer1, *bigdeer2, *bigdeer3, *bigdeer4,
       *lookdeer0, *lookdeer1, *lookdeer2, *lookdeer3, *lookdeer4,
       *w_holiday,
       *w_del_msg;

void main()
{
 void done();
 int loopy;
 chtype noseattr;

  initscr();
  nodelay(stdscr, TRUE);
  noecho();
  nonl();
  refresh();
/*  signal(SIGINT,done);
  signal(SIGTERM,done);
#if !defined	DOS && !defined OS2
  signal(SIGHUP,done);
  signal(SIGQUIT,done);
#endif
*/

#ifdef A_COLOR
  if ( has_colors() )
    {
  	start_color();
  	init_pair(31, COLOR_RED, COLOR_BLACK);
  	noseattr = COLOR_PAIR(31);
    }
  else
  	noseattr = A_NORMAL;
#else
  	noseattr = A_NORMAL;
#endif

#ifdef PDCURSES
  curs_set(0);
#endif

  treescrn = newwin(16,27,3,53);
  treescrn2 = newwin(16,27,3,53);
  treescrn3 = newwin(16,27,3,53);
  treescrn4 = newwin(16,27,3,53);
  treescrn5 = newwin(16,27,3,53);
  treescrn6 = newwin(16,27,3,53);
  treescrn7 = newwin(16,27,3,53);
  treescrn8 = newwin(16,27,3,53);

  dotdeer0 = newwin(3,71,0,8);

  stardeer0 = newwin(4,56,0,8);

  lildeer0 = newwin(7,53,0,8);
  lildeer1 = newwin(2,4,0,0);
  lildeer2 = newwin(2,4,0,0);
  lildeer3 = newwin(2,4,0,0);

  middeer0 = newwin(15,42,0,8);
  middeer1 = newwin(3,7,0,0);
  middeer2 = newwin(3,7,0,0);
  middeer3 = newwin(3,7,0,0);

  bigdeer0 = newwin(10,23,0,0);
  bigdeer1 = newwin(10,23,0,0);
  bigdeer2 = newwin(10,23,0,0);
  bigdeer3 = newwin(10,23,0,0);
  bigdeer4 = newwin(10,23,0,0);

  lookdeer0 = newwin(10,25,0,0);
  lookdeer1 = newwin(10,25,0,0);
  lookdeer2 = newwin(10,25,0,0);
  lookdeer3 = newwin(10,25,0,0);
  lookdeer4 = newwin(10,25,0,0);

  w_holiday = newwin(1,26,3,27);

  w_del_msg = newwin(1,12,23,60);

#if 0
  mvwaddstr(w_del_msg,0,0,"Hit any key to quit");
#endif

  mvwaddstr(w_holiday,0,0,"H A P P Y  H O L I D A Y S");

  /* set up the windows for our various reindeer */

  /* lildeer1 */
  mvwaddch(lildeer1,0,0,(chtype)'V');
  mvwaddch(lildeer1,1,0,(chtype)'@');
  mvwaddch(lildeer1,1,1,(chtype)'<');
  mvwaddch(lildeer1,1,2,(chtype)'>');
  mvwaddch(lildeer1,1,3,(chtype)'~');

  /* lildeer2 */
  mvwaddch(lildeer2,0,0,(chtype)'V');
  mvwaddch(lildeer2,1,0,(chtype)'@');
  mvwaddch(lildeer2,1,1,(chtype)'|');
  mvwaddch(lildeer2,1,2,(chtype)'|');
  mvwaddch(lildeer2,1,3,(chtype)'~');

  /* lildeer3 */
  mvwaddch(lildeer3,0,0,(chtype)'V');
  mvwaddch(lildeer3,1,0,(chtype)'@');
  mvwaddch(lildeer3,1,1,(chtype)'>');
  mvwaddch(lildeer3,1,2,(chtype)'<');
  mvwaddch(lildeer2,1,3,(chtype)'~');


  /* middeer1 */
  mvwaddch(middeer1,0,2,(chtype)'y');
  mvwaddch(middeer1,0,3,(chtype)'y');
  mvwaddch(middeer1,1,2,(chtype)'0');
  mvwaddch(middeer1,1,3,(chtype)'(');
  mvwaddch(middeer1,1,4,(chtype)'=');
  mvwaddch(middeer1,1,5,(chtype)')');
  mvwaddch(middeer1,1,6,(chtype)'~');
  mvwaddch(middeer1,2,3,(chtype)'\\');
  mvwaddch(middeer1,2,4,(chtype)'/');

  /* middeer2 */
  mvwaddch(middeer2,0,2,(chtype)'y');
  mvwaddch(middeer2,0,3,(chtype)'y');
  mvwaddch(middeer2,1,2,(chtype)'0');
  mvwaddch(middeer2,1,3,(chtype)'(');
  mvwaddch(middeer2,1,4,(chtype)'=');
  mvwaddch(middeer2,1,5,(chtype)')');
  mvwaddch(middeer2,1,6,(chtype)'~');
  mvwaddch(middeer2,2,3,(chtype)'|');
  mvwaddch(middeer2,2,5,(chtype)'|');

  /* middeer3 */
  mvwaddch(middeer3,0,2,(chtype)'y');
  mvwaddch(middeer3,0,3,(chtype)'y');
  mvwaddch(middeer3,1,2,(chtype)'0');
  mvwaddch(middeer3,1,3,(chtype)'(');
  mvwaddch(middeer3,1,4,(chtype)'=');
  mvwaddch(middeer3,1,5,(chtype)')');
  mvwaddch(middeer3,1,6,(chtype)'~');
  mvwaddch(middeer3,2,2,(chtype)'/');
  mvwaddch(middeer3,2,6,(chtype)'\\');


  /* bigdeer1 */
  mvwaddch(bigdeer1,0,17,(chtype)'\\');
  mvwaddch(bigdeer1,0,18,(chtype)'/');
  mvwaddch(bigdeer1,0,20,(chtype)'\\');
  mvwaddch(bigdeer1,0,21,(chtype)'/');
  mvwaddch(bigdeer1,1,18,(chtype)'\\');
  mvwaddch(bigdeer1,1,20,(chtype)'/');
  mvwaddch(bigdeer1,2,19,(chtype)'|');
  mvwaddch(bigdeer1,2,20,(chtype)'_');
  mvwaddch(bigdeer1,3,18,(chtype)'/');
  mvwaddch(bigdeer1,3,19,(chtype)'^');
  mvwaddch(bigdeer1,3,20,(chtype)'0');
  mvwaddch(bigdeer1,3,21,(chtype)'\\');
  mvwaddch(bigdeer1,4,17,(chtype)'/');
  mvwaddch(bigdeer1,4,18,(chtype)'/');
  mvwaddch(bigdeer1,4,19,(chtype)'\\');
  mvwaddch(bigdeer1,4,22,(chtype)'\\');
  mvwaddstr(bigdeer1,5,7,"^~~~~~~~~//  ~~U");
  mvwaddstr(bigdeer1,6,7,"( \\_____( /");
  mvwaddstr(bigdeer1,7,8,"( )    /");
  mvwaddstr(bigdeer1,8,9,"\\\\   /");
  mvwaddstr(bigdeer1,9,11,"\\>/>");

  /* bigdeer2 */
  mvwaddch(bigdeer2,0,17,(chtype)'\\');
  mvwaddch(bigdeer2,0,18,(chtype)'/');
  mvwaddch(bigdeer2,0,20,(chtype)'\\');
  mvwaddch(bigdeer2,0,21,(chtype)'/');
  mvwaddch(bigdeer2,1,18,(chtype)'\\');
  mvwaddch(bigdeer2,1,20,(chtype)'/');
  mvwaddch(bigdeer2,2,19,(chtype)'|');
  mvwaddch(bigdeer2,2,20,(chtype)'_');
  mvwaddch(bigdeer2,3,18,(chtype)'/');
  mvwaddch(bigdeer2,3,19,(chtype)'^');
  mvwaddch(bigdeer2,3,20,(chtype)'0');
  mvwaddch(bigdeer2,3,21,(chtype)'\\');
  mvwaddch(bigdeer2,4,17,(chtype)'/');
  mvwaddch(bigdeer2,4,18,(chtype)'/');
  mvwaddch(bigdeer2,4,19,(chtype)'\\');
  mvwaddch(bigdeer2,4,22,(chtype)'\\');
  mvwaddstr(bigdeer2,5,7,"^~~~~~~~~//  ~~U");
  mvwaddstr(bigdeer2,6,7,"(( )____( /");
  mvwaddstr(bigdeer2,7,7,"( /      |");
  mvwaddstr(bigdeer2,8,8,"\\/      |");
  mvwaddstr(bigdeer2,9,9,"|>     |>");

  /* bigdeer3 */
  mvwaddch(bigdeer3,0,17,(chtype)'\\');
  mvwaddch(bigdeer3,0,18,(chtype)'/');
  mvwaddch(bigdeer3,0,20,(chtype)'\\');
  mvwaddch(bigdeer3,0,21,(chtype)'/');
  mvwaddch(bigdeer3,1,18,(chtype)'\\');
  mvwaddch(bigdeer3,1,20,(chtype)'/');
  mvwaddch(bigdeer3,2,19,(chtype)'|');
  mvwaddch(bigdeer3,2,20,(chtype)'_');
  mvwaddch(bigdeer3,3,18,(chtype)'/');
  mvwaddch(bigdeer3,3,19,(chtype)'^');
  mvwaddch(bigdeer3,3,20,(chtype)'0');
  mvwaddch(bigdeer3,3,21,(chtype)'\\');
  mvwaddch(bigdeer3,4,17,(chtype)'/');
  mvwaddch(bigdeer3,4,18,(chtype)'/');
  mvwaddch(bigdeer3,4,19,(chtype)'\\');
  mvwaddch(bigdeer3,4,22,(chtype)'\\');
  mvwaddstr(bigdeer3,5,7,"^~~~~~~~~//  ~~U");
  mvwaddstr(bigdeer3,6,6,"( ()_____( /");
  mvwaddstr(bigdeer3,7,6,"/ /       /");
  mvwaddstr(bigdeer3,8,5,"|/          \\");
  mvwaddstr(bigdeer3,9,5,"/>           \\>");

  /* bigdeer4 */
  mvwaddch(bigdeer4,0,17,(chtype)'\\');
  mvwaddch(bigdeer4,0,18,(chtype)'/');
  mvwaddch(bigdeer4,0,20,(chtype)'\\');
  mvwaddch(bigdeer4,0,21,(chtype)'/');
  mvwaddch(bigdeer4,1,18,(chtype)'\\');
  mvwaddch(bigdeer4,1,20,(chtype)'/');
  mvwaddch(bigdeer4,2,19,(chtype)'|');
  mvwaddch(bigdeer4,2,20,(chtype)'_');
  mvwaddch(bigdeer4,3,18,(chtype)'/');
  mvwaddch(bigdeer4,3,19,(chtype)'^');
  mvwaddch(bigdeer4,3,20,(chtype)'0');
  mvwaddch(bigdeer4,3,21,(chtype)'\\');
  mvwaddch(bigdeer4,4,17,(chtype)'/');
  mvwaddch(bigdeer4,4,18,(chtype)'/');
  mvwaddch(bigdeer4,4,19,(chtype)'\\');
  mvwaddch(bigdeer4,4,22,(chtype)'\\');
  mvwaddstr(bigdeer4,5,7,"^~~~~~~~~//  ~~U");
  mvwaddstr(bigdeer4,6,6,"( )______( /");
  mvwaddstr(bigdeer4,7,5,"(/          \\");
  mvwaddstr(bigdeer4,8,0,"v___=             ----^");

  /* lookdeer1 */
  mvwaddstr(lookdeer1,0,16,"\\/     \\/");
  mvwaddstr(lookdeer1,1,17,"\\Y/ \\Y/");
  mvwaddstr(lookdeer1,2,19,"\\=/");
  mvwaddstr(lookdeer1,3,17,"^\\o o/^");
  mvwaddstr(lookdeer1,4,17,"//( )");
  mvwaddstr(lookdeer1,5,7,"^~~~~~~~~// \\");
  waddch(lookdeer1,'O' | noseattr);
  waddstr(lookdeer1,"/");
  mvwaddstr(lookdeer1,6,7,"( \\_____( /");
  mvwaddstr(lookdeer1,7,8,"( )    /");
  mvwaddstr(lookdeer1,8,9,"\\\\   /");
  mvwaddstr(lookdeer1,9,11,"\\>/>");

  /* lookdeer2 */
  mvwaddstr(lookdeer2,0,16,"\\/     \\/");
  mvwaddstr(lookdeer2,1,17,"\\Y/ \\Y/");
  mvwaddstr(lookdeer2,2,19,"\\=/");
  mvwaddstr(lookdeer2,3,17,"^\\o o/^");
  mvwaddstr(lookdeer2,4,17,"//( )");
  mvwaddstr(lookdeer2,5,7,"^~~~~~~~~// \\");
  waddch(lookdeer2,'O' | noseattr);
  waddstr(lookdeer2,"/");
  mvwaddstr(lookdeer2,6,7,"(( )____( /");
  mvwaddstr(lookdeer2,7,7,"( /      |");
  mvwaddstr(lookdeer2,8,8,"\\/      |");
  mvwaddstr(lookdeer2,9,9,"|>     |>");

  /* lookdeer3 */
  mvwaddstr(lookdeer3,0,16,"\\/     \\/");
  mvwaddstr(lookdeer3,1,17,"\\Y/ \\Y/");
  mvwaddstr(lookdeer3,2,19,"\\=/");
  mvwaddstr(lookdeer3,3,17,"^\\o o/^");
  mvwaddstr(lookdeer3,4,17,"//( )");
  mvwaddstr(lookdeer3,5,7,"^~~~~~~~~// \\");
  waddch(lookdeer3,'O' | noseattr);
  waddstr(lookdeer3,"/");
  mvwaddstr(lookdeer3,6,6,"( ()_____( /");
  mvwaddstr(lookdeer3,7,6,"/ /       /");
  mvwaddstr(lookdeer3,8,5,"|/          \\");
  mvwaddstr(lookdeer3,9,5,"/>           \\>");

  /* lookdeer4 */
  mvwaddstr(lookdeer4,0,16,"\\/     \\/");
  mvwaddstr(lookdeer4,1,17,"\\Y/ \\Y/");
  mvwaddstr(lookdeer4,2,19,"\\=/");
  mvwaddstr(lookdeer4,3,17,"^\\o o/^");
  mvwaddstr(lookdeer4,4,17,"//( )");
  mvwaddstr(lookdeer4,5,7,"^~~~~~~~~// \\");
  waddch(lookdeer4,'O' | noseattr);
  waddstr(lookdeer4,"/");
  mvwaddstr(lookdeer4,6,6,"( )______( /");
  mvwaddstr(lookdeer4,7,5,"(/          \\");
  mvwaddstr(lookdeer4,8,0,"v___=             ----^");



  /***********************************************/
#if 0
  do
  {
#endif
    clear();
    werase(treescrn);
    touchwin(treescrn);
    werase(treescrn2);
    touchwin(treescrn2);
    werase(treescrn8);
    touchwin(treescrn8);
    refresh();
    delay_output(1000);
    boxit();
    del_msg();
    delay_output(1000);
    seas();
    del_msg();
    delay_output(1000);
    greet();
    del_msg();
    delay_output(1000);
    fromwho();
    del_msg();
    delay_output(1000);
    tree();
    delay_output(1000);
    balls();
    delay_output(1000);
    star();
    delay_output(1000);
    strng1();
    strng2();
    strng3();
    strng4();
    strng5();


  /* set up the windows for our blinking trees */
  /* **************************************** */
  /* treescrn3 */

               overlay(treescrn, treescrn3);

             /*balls*/
               mvwaddch(treescrn3, 4, 18, ' ');
               mvwaddch(treescrn3, 7, 6, ' ');
               mvwaddch(treescrn3, 8, 19, ' ');
               mvwaddch(treescrn3, 11, 22, ' ');

             /*star*/
               mvwaddch(treescrn3, 0, 12, '*');

             /*strng1*/
               mvwaddch(treescrn3, 3, 11, ' ');

             /*strng2*/
               mvwaddch(treescrn3, 5, 13, ' ');
               mvwaddch(treescrn3, 6, 10, ' ');

             /*strng3*/
               mvwaddch(treescrn3, 7, 16, ' ');
               mvwaddch(treescrn3, 7, 14, ' ');

             /*strng4*/
               mvwaddch(treescrn3, 10, 13, ' ');
               mvwaddch(treescrn3, 10, 10, ' ');
               mvwaddch(treescrn3, 11, 8, ' ');

             /*strng5*/
               mvwaddch(treescrn3, 11, 18, ' ');
               mvwaddch(treescrn3, 12, 13, ' ');


  /* treescrn4 */

               overlay(treescrn, treescrn4);

             /*balls*/
               mvwaddch(treescrn4, 3, 9, ' ');
               mvwaddch(treescrn4, 4, 16, ' ');
               mvwaddch(treescrn4, 7, 6, ' ');
               mvwaddch(treescrn4, 8, 19, ' ');
               mvwaddch(treescrn4, 11, 2, ' ');
               mvwaddch(treescrn4, 12, 23, ' ');

             /*star*/
               mvwaddch(treescrn4, 0, 12, '*' | A_STANDOUT );

             /*strng1*/
               mvwaddch(treescrn4, 3, 13, ' ');

             /*strng2*/

	     /*strng3*/
               mvwaddch(treescrn4, 7, 15, ' ');
               mvwaddch(treescrn4, 8, 11, ' ');

             /*strng4*/
               mvwaddch(treescrn4, 9, 16, ' ');
               mvwaddch(treescrn4, 10, 12, ' ');
               mvwaddch(treescrn4, 11, 8, ' ');

             /*strng5*/
               mvwaddch(treescrn4, 11, 18, ' ');
	       mvwaddch(treescrn4, 12, 14, ' ');


  /* treescrn5 */

               overlay(treescrn, treescrn5);

             /*balls*/
               mvwaddch(treescrn5, 3, 15, ' ');
               mvwaddch(treescrn5, 10, 20, ' ');
               mvwaddch(treescrn5, 12, 1, ' ');

             /*star*/
               mvwaddch(treescrn5, 0, 12, '*');

             /*strng1*/
               mvwaddch(treescrn5, 3, 11, ' ');

             /*strng2*/
               mvwaddch(treescrn5, 5, 12, ' ');

             /*strng3*/
	       mvwaddch(treescrn5, 7, 14, ' ');
               mvwaddch(treescrn5, 8, 10, ' ');

             /*strng4*/
               mvwaddch(treescrn5, 9, 15, ' ');
               mvwaddch(treescrn5, 10, 11, ' ');
               mvwaddch(treescrn5, 11, 7, ' ');

             /*strng5*/
               mvwaddch(treescrn5, 11, 17, ' ');
               mvwaddch(treescrn5, 12, 13, ' ');

  /* treescrn6 */

               overlay(treescrn, treescrn6);

             /*balls*/
               mvwaddch(treescrn6, 6, 7, ' ');
               mvwaddch(treescrn6, 7, 18, ' ');
               mvwaddch(treescrn6, 10, 4, ' ');
               mvwaddch(treescrn6, 11, 23, ' ');

	     /*star*/
               mvwaddch(treescrn6, 0, 12, '*' | A_STANDOUT );

             /*strng1*/

             /*strng2*/
               mvwaddch(treescrn6, 5, 11, ' ');

             /*strng3*/
	       mvwaddch(treescrn6, 7, 13, ' ');
               mvwaddch(treescrn6, 8, 9, ' ');

             /*strng4*/
               mvwaddch(treescrn6, 9, 14, ' ');
               mvwaddch(treescrn6, 10, 10, ' ');
               mvwaddch(treescrn6, 11, 6, ' ');

             /*strng5*/
               mvwaddch(treescrn6, 11, 16, ' ');
               mvwaddch(treescrn6, 12, 12, ' ');

  /* treescrn7 */

               overlay(treescrn, treescrn7);

             /*balls*/
               mvwaddch(treescrn7, 3, 15, ' ');
               mvwaddch(treescrn7, 6, 7, ' ');
               mvwaddch(treescrn7, 7, 18, ' ');
               mvwaddch(treescrn7, 10, 4, ' ');
               mvwaddch(treescrn7, 11, 22, ' ');

             /*star*/
               mvwaddch(treescrn7, 0, 12, '*');

             /*strng1*/
               mvwaddch(treescrn7, 3, 12, ' ');

             /*strng2*/
               mvwaddch(treescrn7, 5, 13, ' ');
               mvwaddch(treescrn7, 6, 9, ' ');

	     /*strng3*/
               mvwaddch(treescrn7, 7, 15, ' ');
               mvwaddch(treescrn7, 8, 11, ' ');

             /*strng4*/
               mvwaddch(treescrn7, 9, 16, ' ');
               mvwaddch(treescrn7, 10, 12, ' ');
               mvwaddch(treescrn7, 11, 8, ' ');

             /*strng5*/
               mvwaddch(treescrn7, 11, 18, ' ');
	       mvwaddch(treescrn7, 12, 14, ' ');


    delay_output(1000);
    reindeer();

    touchwin(w_holiday);
    wrefresh(w_holiday);
    wrefresh(w_del_msg);

    delay_output(1000);
    for(loopy = 0;loopy < 50;loopy++)
    {
      blinkit();
    }

    done();

#if 0
  }
  while(getch() == (ERR));
/*  while(!typeahead(stdin));*/
#endif
}
int
boxit()
{
 int x = 0;

  while(x < 20)
  {
    mvaddch(x, 7, '|');
    ++x;
  }

  x = 8;

  while(x < 80)
  {
    mvaddch(19, x, '_');
    ++x;
  }

  x = 0;

  while(x < 80)
  {
    mvaddch(22, x, '_');
    ++x;
  }

  return( 0 );
}

int
seas()
{
  mvaddch(4, 1, 'S');
  mvaddch(6, 1, 'E');
  mvaddch(8, 1, 'A');
  mvaddch(10, 1, 'S');
  mvaddch(12, 1, 'O');
  mvaddch(14, 1, 'N');
  mvaddch(16, 1, '`');
  mvaddch(18, 1, 'S');

  return( 0 );
}

int
greet()
{
  mvaddch(3, 5, 'G');
  mvaddch(5, 5, 'R');
  mvaddch(7, 5, 'E');
  mvaddch(9, 5, 'E');
  mvaddch(11, 5, 'T');
  mvaddch(13, 5, 'I');
  mvaddch(15, 5, 'N');
  mvaddch(17, 5, 'G');
  mvaddch(19, 5, 'S');

  return( 0 );
}

int
fromwho()
{
  mvaddstr(21, 13, FROMWHO);
  return( 0 );
}

int
del_msg()
{
#if 0
  mvaddstr(23, 60, "Hit any key to quit");
#endif

  refresh();

  return( 0 );
}

int
tree()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(30,COLOR_GREEN,COLOR_BLACK);
     wattrset(treescrn,COLOR_PAIR(30));
    }
#endif
  mvwaddch(treescrn, 1, 11, (chtype)'/');
  mvwaddch(treescrn, 2, 11, (chtype)'/');
  mvwaddch(treescrn, 3, 10, (chtype)'/');
  mvwaddch(treescrn, 4, 9, (chtype)'/');
  mvwaddch(treescrn, 5, 9, (chtype)'/');
  mvwaddch(treescrn, 6, 8, (chtype)'/');
  mvwaddch(treescrn, 7, 7, (chtype)'/');
  mvwaddch(treescrn, 8, 6, (chtype)'/');
  mvwaddch(treescrn, 9, 6, (chtype)'/');
  mvwaddch(treescrn, 10, 5, (chtype)'/');
  mvwaddch(treescrn, 11, 3, (chtype)'/');
  mvwaddch(treescrn, 12, 2, (chtype)'/');

  mvwaddch(treescrn, 1, 13, (chtype)'\\');
  mvwaddch(treescrn, 2, 13, (chtype)'\\');
  mvwaddch(treescrn, 3, 14, (chtype)'\\');
  mvwaddch(treescrn, 4, 15, (chtype)'\\');
  mvwaddch(treescrn, 5, 15, (chtype)'\\');
  mvwaddch(treescrn, 6, 16, (chtype)'\\');
  mvwaddch(treescrn, 7, 17, (chtype)'\\');
  mvwaddch(treescrn, 8, 18, (chtype)'\\');
  mvwaddch(treescrn, 9, 18, (chtype)'\\');
  mvwaddch(treescrn, 10, 19, (chtype)'\\');
  mvwaddch(treescrn, 11, 21, (chtype)'\\');
  mvwaddch(treescrn, 12, 22, (chtype)'\\');

  mvwaddch(treescrn, 4, 10, (chtype)'_');
  mvwaddch(treescrn, 4, 14, (chtype)'_');
  mvwaddch(treescrn, 8, 7, (chtype)'_');
  mvwaddch(treescrn, 8, 17, (chtype)'_');

  mvwaddstr(treescrn, 13, 0, "//////////// \\\\\\\\\\\\\\\\\\\\\\\\");

#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(20,COLOR_YELLOW,COLOR_BLACK);
     wattrset(treescrn,COLOR_PAIR(20));
    }
#endif
  mvwaddstr(treescrn, 14, 11, "| |");
  mvwaddstr(treescrn, 15, 11, "|_|");

  wrefresh(treescrn);
  wrefresh(w_del_msg);

  return( 0 );
}

int
balls()
{
  chtype ball1,ball2,ball3,ball4,ball5,ball6;
  overlay(treescrn, treescrn2);

#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(1, COLOR_BLUE,COLOR_BLACK);
     init_pair(2, COLOR_RED,COLOR_BLACK);
     init_pair(3, COLOR_MAGENTA,COLOR_BLACK);
     init_pair(4, COLOR_CYAN,COLOR_BLACK);
     init_pair(5, COLOR_YELLOW,COLOR_BLACK);
     init_pair(6, COLOR_WHITE,COLOR_BLACK);
     ball1 = COLOR_PAIR(1);
     ball2 = COLOR_PAIR(2);
     ball3 = COLOR_PAIR(3);
     ball4 = COLOR_PAIR(4);
     ball5 = COLOR_PAIR(5);
     ball6 = COLOR_PAIR(6);
    }
  else
     ball1 = ball2 = ball3 = ball4 = ball5 = ball6 = 0;
#else
  ball1 = ball2 = ball3 = ball4 = ball5 = ball6 = 0;
#endif

  mvwaddch(treescrn2, 3, 9, (chtype)'@'|ball1);
  mvwaddch(treescrn2, 3, 15, (chtype)'@'|ball2);
  mvwaddch(treescrn2, 4, 8, (chtype)'@'|ball3);
  mvwaddch(treescrn2, 4, 16, (chtype)'@'|ball4);
  mvwaddch(treescrn2, 5, 7, (chtype)'@'|ball5);
  mvwaddch(treescrn2, 5, 17, (chtype)'@'|ball6);
  mvwaddch(treescrn2, 7, 6, (chtype)'@'|ball1|A_BOLD);
  mvwaddch(treescrn2, 7, 18, (chtype)'@'|ball2|A_BOLD);
  mvwaddch(treescrn2, 8, 5, (chtype)'@'|ball3|A_BOLD);
  mvwaddch(treescrn2, 8, 19, (chtype)'@'|ball4|A_BOLD);
  mvwaddch(treescrn2, 10, 4, (chtype)'@'|ball5|A_BOLD);
  mvwaddch(treescrn2, 10, 20, (chtype)'@'|ball6|A_BOLD);
  mvwaddch(treescrn2, 11, 2, (chtype)'@'|ball1);
  mvwaddch(treescrn2, 11, 22, (chtype)'@'|ball2);
  mvwaddch(treescrn2, 12, 1, (chtype)'@'|ball3);
  mvwaddch(treescrn2, 12, 23, (chtype)'@'|ball4);

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
star()
{
  mvwaddch(treescrn2, 0, 12, (chtype)'*' | A_STANDOUT );

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
strng1()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(10, COLOR_YELLOW,COLOR_BLACK);
     wattrset(treescrn2,COLOR_PAIR(10)|A_BOLD);
    }
#endif
  mvwaddch(treescrn2, 3, 13, (chtype)'\'');
  mvwaddch(treescrn2, 3, 12, (chtype)':');
  mvwaddch(treescrn2, 3, 11, (chtype)'.');

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
strng2()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(11, COLOR_RED,COLOR_BLACK);
     wattrset(treescrn2,COLOR_PAIR(11)|A_BOLD);
    }
#endif
  mvwaddch(treescrn2, 5, 14, (chtype)'\'');
  mvwaddch(treescrn2, 5, 13, (chtype)':');
  mvwaddch(treescrn2, 5, 12, (chtype)'.');
  mvwaddch(treescrn2, 5, 11, (chtype)',');
  mvwaddch(treescrn2, 6, 10, (chtype)'\'');
  mvwaddch(treescrn2, 6, 9, (chtype)':');

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
strng3()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(12, COLOR_GREEN,COLOR_BLACK);
     wattrset(treescrn2,COLOR_PAIR(12)|A_BOLD);
    }
#endif
  mvwaddch(treescrn2, 7, 16, (chtype)'\'');
  mvwaddch(treescrn2, 7, 15, (chtype)':');
  mvwaddch(treescrn2, 7, 14, (chtype)'.');
  mvwaddch(treescrn2, 7, 13, (chtype)',');
  mvwaddch(treescrn2, 8, 12, (chtype)'\'');
  mvwaddch(treescrn2, 8, 11, (chtype)':');
  mvwaddch(treescrn2, 8, 10, (chtype)'.');
  mvwaddch(treescrn2, 8, 9, (chtype)',');

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
strng4()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(13, COLOR_WHITE,COLOR_BLACK);
     wattrset(treescrn2,COLOR_PAIR(13)|A_BOLD);
    }
#endif
  mvwaddch(treescrn2, 9, 17, (chtype)'\'');
  mvwaddch(treescrn2, 9, 16, (chtype)':');
  mvwaddch(treescrn2, 9, 15, (chtype)'.');
  mvwaddch(treescrn2, 9, 14, (chtype)',');
  mvwaddch(treescrn2, 10, 13, (chtype)'\'');
  mvwaddch(treescrn2, 10, 12, (chtype)':');
  mvwaddch(treescrn2, 10, 11, (chtype)'.');
  mvwaddch(treescrn2, 10, 10, (chtype)',');
  mvwaddch(treescrn2, 11, 9, (chtype)'\'');
  mvwaddch(treescrn2, 11, 8, (chtype)':');
  mvwaddch(treescrn2, 11, 7, (chtype)'.');
  mvwaddch(treescrn2, 11, 6, (chtype)',');
  mvwaddch(treescrn2, 12, 5, (chtype)'\'');

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
strng5()
{
#ifdef A_COLOR
  if (has_colors() )
    {
     init_pair(14, COLOR_CYAN,COLOR_BLACK);
     wattrset(treescrn2,COLOR_PAIR(14)|A_BOLD);
    }
#endif
  mvwaddch(treescrn2, 11, 19, (chtype)'\'');
  mvwaddch(treescrn2, 11, 18, (chtype)':');
  mvwaddch(treescrn2, 11, 17, (chtype)'.');
  mvwaddch(treescrn2, 11, 16, (chtype)',');
  mvwaddch(treescrn2, 12, 15, (chtype)'\'');
  mvwaddch(treescrn2, 12, 14, (chtype)':');
  mvwaddch(treescrn2, 12, 13, (chtype)'.');
  mvwaddch(treescrn2, 12, 12, (chtype)',');

  /* save a fully lit tree */
  overlay(treescrn2, treescrn);

  wrefresh(treescrn2);
  wrefresh(w_del_msg);
  return( 0 );
}

int
blinkit()
{
 static int cycle;

  if(cycle > 4)
  {
    cycle = 0;
  }


  touchwin(treescrn8);

  switch(cycle)
  {

    case 0:
               overlay(treescrn3, treescrn8);
               wrefresh(treescrn8);
               wrefresh(w_del_msg);

               break;
    case 1:
               overlay(treescrn4, treescrn8);
               wrefresh(treescrn8);
               wrefresh(w_del_msg);

               break;
    case 2:
               overlay(treescrn5, treescrn8);
               wrefresh(treescrn8);
               wrefresh(w_del_msg);

               break;
    case 3:
               overlay(treescrn6, treescrn8);
               wrefresh(treescrn8);
               wrefresh(w_del_msg);

               break;
    case 4:
               overlay(treescrn7, treescrn8);
               wrefresh(treescrn8);
               wrefresh(w_del_msg);

               break;
  }

   napms(50);
   touchwin(treescrn8);



   /*ALL ON***************************************************/


   overlay(treescrn, treescrn8);
   wrefresh(treescrn8);
   wrefresh(w_del_msg);


  ++cycle;
  return( 0 );
}

int
reindeer()
{
 int looper;

  y_pos = 0;


  for(x_pos = 70; x_pos > 62; x_pos--)
  {
    if(x_pos < 62)
    {
      y_pos = 1;
    }
    for(looper = 0; looper < 4; looper++)
    {
      mvwaddch(dotdeer0, y_pos, x_pos, (chtype)'.');
      wrefresh(dotdeer0);
      wrefresh(w_del_msg);
      werase(dotdeer0);
      wrefresh(dotdeer0);
      wrefresh(w_del_msg);
    }
  }

  y_pos = 2;

  for(; x_pos > 50; x_pos--)
  {

    for(looper = 0; looper < 4; looper++)
    {

      if(x_pos < 56)
      {
        y_pos = 3;

        mvwaddch(stardeer0, y_pos, x_pos, (chtype)'*');
        wrefresh(stardeer0);
        wrefresh(w_del_msg);
        werase(stardeer0);
        wrefresh(stardeer0);
        wrefresh(w_del_msg);
      }
      else
      {
        mvwaddch(dotdeer0, y_pos, x_pos, (chtype)'*');
        wrefresh(dotdeer0);
        wrefresh(w_del_msg);
        werase(dotdeer0);
        wrefresh(dotdeer0);
        wrefresh(w_del_msg);
      }
    }
  }

  x_pos = 58;

  for(y_pos = 2; y_pos < 5; y_pos++)
  {

    touchwin(lildeer0);
    wrefresh(lildeer0);
    wrefresh(w_del_msg);
   napms(50);

    for(looper = 0; looper < 4; looper++)
    {
      mvwin(lildeer3, y_pos, x_pos);
      wrefresh(lildeer3);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(lildeer2, y_pos, x_pos);
      wrefresh(lildeer2);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(lildeer1, y_pos, x_pos);
      wrefresh(lildeer1);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(lildeer2, y_pos, x_pos);
      wrefresh(lildeer2);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(lildeer3, y_pos, x_pos);
      wrefresh(lildeer3);
      wrefresh(w_del_msg);
   napms(50);

      touchwin(lildeer0);
      wrefresh(lildeer0);
      wrefresh(w_del_msg);
   napms(50);

      x_pos -= 2;
    }
  }


  x_pos = 35;

  for(y_pos = 5; y_pos < 10; y_pos++)
  {

    touchwin(middeer0);
    wrefresh(middeer0);
    wrefresh(w_del_msg);

    for(looper = 0; looper < 2; looper++)
    {
      mvwin(middeer3, y_pos, x_pos);
      wrefresh(middeer3);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(middeer2, y_pos, x_pos);
      wrefresh(middeer2);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(middeer1, y_pos, x_pos);
      wrefresh(middeer1);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(middeer2, y_pos, x_pos);
      wrefresh(middeer2);
      wrefresh(w_del_msg);
   napms(50);

      mvwin(middeer3, y_pos, x_pos);
      wrefresh(middeer3);
      wrefresh(w_del_msg);
   napms(50);

      touchwin(middeer0);
      wrefresh(middeer0);
      wrefresh(w_del_msg);
   napms(50);

      x_pos -= 3;
    }
  }

  delay_output(2000);

  y_pos = 1;

  for(x_pos = 8; x_pos < 16; x_pos++)
  {

      mvwin(bigdeer4, y_pos, x_pos);
      wrefresh(bigdeer4);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer3, y_pos, x_pos);
      wrefresh(bigdeer3);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer2, y_pos, x_pos);
      wrefresh(bigdeer2);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer1, y_pos, x_pos);
      wrefresh(bigdeer1);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer2, y_pos, x_pos);
      wrefresh(bigdeer2);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer3, y_pos, x_pos);
      wrefresh(bigdeer3);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer4, y_pos, x_pos);
      wrefresh(bigdeer4);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer0, y_pos, x_pos);
      wrefresh(bigdeer0);
      wrefresh(w_del_msg);
   napms(30);
  }

    --x_pos;

    for(looper = 0; looper < 6; looper++)
    {
      mvwin(lookdeer4, y_pos, x_pos);
      wrefresh(lookdeer4);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer3, y_pos, x_pos);
      wrefresh(lookdeer3);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer2, y_pos, x_pos);
      wrefresh(lookdeer2);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer1, y_pos, x_pos);
      wrefresh(lookdeer1);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer2, y_pos, x_pos);
      wrefresh(lookdeer2);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer3, y_pos, x_pos);
      wrefresh(lookdeer3);
      wrefresh(w_del_msg);
   napms(40);

      mvwin(lookdeer4, y_pos, x_pos);
      wrefresh(lookdeer4);
      wrefresh(w_del_msg);
   napms(40);

    }

    mvwin(lookdeer0, y_pos, x_pos);
    wrefresh(lookdeer0);
    wrefresh(w_del_msg);
   napms(40);

  for(; y_pos < 10; y_pos++)
  {

    for(looper = 0; looper < 2; looper++)
    {
      mvwin(bigdeer4, y_pos, x_pos);
      wrefresh(bigdeer4);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer3, y_pos, x_pos);
      wrefresh(bigdeer3);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer2, y_pos, x_pos);
      wrefresh(bigdeer2);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer1, y_pos, x_pos);
      wrefresh(bigdeer1);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer2, y_pos, x_pos);
      wrefresh(bigdeer2);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer3, y_pos, x_pos);
      wrefresh(bigdeer3);
      wrefresh(w_del_msg);
   napms(30);

      mvwin(bigdeer4, y_pos, x_pos);
      wrefresh(bigdeer4);
      wrefresh(w_del_msg);
   napms(30);
    }
      mvwin(bigdeer0, y_pos, x_pos);
      wrefresh(bigdeer0);
      wrefresh(w_del_msg);
   napms(30);
  }

  --y_pos;

  mvwin(lookdeer3, y_pos, x_pos);
  wrefresh(lookdeer3);
  wrefresh(w_del_msg);
  return( 0 );
}

void done()
{
/*  signal(SIGINT,done);
  signal(SIGTERM,done);
#if !defined	DOS && !defined OS2
  signal(SIGHUP,done);
  signal(SIGQUIT,done);
#endif*/
  clear();
  refresh();
  endwin();
#ifdef XCURSES
  XCursesExit();
#endif
  exit(0);
}
