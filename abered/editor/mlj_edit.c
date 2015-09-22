/*
 * This is a general purpose editing function.  It returns a string.
 * by: James Willie (jwillie@werple.apana.org.au)
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pc.h>
#include <keys.h>
#include <gppconio.h>

#include "mlj_edit.h"

#define BUFFER_LINES	400      /* Max number if lines read in     */
#define LINELEN		100	 /* How much allocate for each line */
#define START_ROW	3
#define SCREEN(X)	(X+START_ROW-top_row) /* map buffer row to screen */
#define bot_row		(top_row+20)

static void beep();
static void insert_line(int r);
static void delete_line(int r);
static void init_buffer();
static void string2buffer(char *s);
static void buffer2string(char *s);
static void handle_input();
static void show_info();
static void do_redraw_screen();
static void do_draw_header();
static void help_screen();

unsigned char word_wrap;

static int aborted;
static int row;
static int col;
static int max_row;
static int top_row;
static int store_row;
static int store_col;
static short store_screen[4096];
static char buffer[BUFFER_LINES][LINELEN];
static unsigned char cols[BUFFER_LINES];
static char header[100] = "\t\tEdit string by J.Willie. Copyright (C) 1994";

char *edit_string(char *buf, int create_mem)
{
  char *p;

  aborted = row = col = max_row = top_row = 0;

  init_buffer();
  string2buffer(buf);

  /* Save current screen and setup edit screen */
  ScreenRetrieve(store_screen);
  ScreenGetCursor(&store_row, &store_col);
  ScreenClear();
  ScreenSetCursor(0, 0);
  do_draw_header();
  do_redraw_screen();
  ScreenSetCursor(SCREEN(row), col);
  handle_input();
    
  /* Restore the old screen */
  ScreenUpdate(store_screen);
  ScreenSetCursor(store_row, store_col);

  if (create_mem == 0)
  {
    buffer2string(buf);
    p = buf;
  }
  else
  {
    int i, size;

    for (i = 0, size = 0; i <= max_row; i++)
      size += cols[i];
    p = (char *)malloc(size+1);
    buffer2string(p);
  }

  return ((aborted == 1) ? (char *)NULL : p);
}  

/*
**  This is the REAL guts of the program.  Handles all the input and decides
**  what should be done with it.  The function is invisible to the program
**  using edit_string()
*/
static void handle_input()
{
  int c;
  int i, j;
  int start_word = -1;
  int in_word = 0;

  while(1)
  {
    show_info();
    ScreenSetCursor(SCREEN(row), col);
    c = getxkey();

    if ((in_word == 0) && !isspace(c))
    {
      in_word = 1;
      start_word = col;
    }
    else if (isspace(c))
    {
      start_word = -1;
      in_word = 0;
    }
        
    switch (c)
    {
      case K_BackSpace:           
      case K_Delete:              
        if (col <= 0)  /* Need to join lines */
        {
          if (row > START_ROW)
          {
            row--;
            col = cols[row];

            /* Copy start of next line to end of this one */
            for (i = 0; (col <= 78); i++, col++)
            {
              buffer[row][col] = buffer[row+1][i];
              if (buffer[row+1][i] == '\0')
                break;
            }

            if (buffer[row+1][i] == '\0')
            {
              delete_line(row+1);
            }
            else
            {
              i--;
              if (buffer[row][col-1] != '\n')
              {
                buffer[row][col] = '\n';
                buffer[row][col+1] = '\0';
              }
              else
              {
                buffer[row][col] = '\0';
                col--;
              }
              /* Copy Rest of the next line, to start of line */
              for (j = 0; buffer[row+1][i] != '\0'; j++, i++)
              {
                buffer[row+1][j] = buffer[row+1][i];
              }    
              cols[row+1] = j-1;
              for (; j < 80; j++)
                buffer[row+1][j] = '\0';
            }

            i = cols[row];    /* Remember where we want the cursor */
            cols[row] = col;  /* Set new line length */
            col = i; 
            do_redraw_screen();
          }
          else
          {
            beep();
          }
        }
        else
        {
          for (i = col; buffer[row][i] != '\0'; i++)
          {
            buffer[row][i-1] = buffer[row][i];
            if (buffer[row][i] == '\n')
              ScreenPutChar(' ', 7, i-1, SCREEN(row));
            else
              ScreenPutChar(buffer[row][i], 7, i-1, SCREEN(row));
          }
          ScreenPutChar(' ', 7, i, SCREEN(row));
          buffer[row][i-1] = '\0';
          col--;
          cols[row] -= 1;
        }
        break;

      case K_Tab:                 
      case K_Escape:              

      case K_F2:                  
      case K_F3:                  
      case K_F4:                  
      case K_F5:                  
      case K_F6:                  
      case K_F7:                  
      case K_F8:                  
      case K_F9:                  
      case K_F10:                 
      case K_F11:                 
      case K_F12:                 
      case K_EHome:               
      case K_EPageUp:             
      case K_EEnd:                
      case K_EPageDown:           
      case K_EInsert:             
      case K_EDelete:             
        break;

      case K_EUp:                 
      case K_Up:                  
        if (row > START_ROW)
        {
          row--;
          if (col > cols[row])
            col = cols[row];
          if (row < top_row)
          {
            top_row = row;
            do_redraw_screen();
          }
        }
        else
        {
          beep();
        }
        break;

      case K_Left:                
      case K_Eleft:               
        if (col <= 0)
        {
          if (row > START_ROW)
          {
            row--;
            col = cols[row];
          }
          else
          {
            beep();
          }
        }
        else
        {
          col--;
        }
        break;

      case K_Right:               
      case K_ERight:              
        if ((col >= 78) || (col >= cols[row]))
        {
          if (row == max_row)
          {
            
          }
          else
          {
            col = 0;
            row++;
          }
        }
        else
          col++;
        break;
        
      case K_Down:                
      case K_EDown:               
        if (row < max_row)
        {
          row++;
          if (cols[row] < col)
            col = cols[row];
          if (row >= bot_row)
          {
            top_row++;
            do_redraw_screen();
          }
        }
        else
        {
          beep();
        }
        break;

      case K_PageUp:              
      case K_Home:                
      case K_Center:              
      case K_End:                 
      case K_PageDown:            
      case K_Insert:              
      case K_Shift_F2:            
      case K_Shift_F3:            
      case K_Shift_F4:            
      case K_Shift_F5:            
      case K_Shift_F6:            
      case K_Shift_F7:            
      case K_Shift_F8:            
      case K_Shift_F9:            
      case K_Shift_F10:           
      case K_Shift_F11:           
      case K_Shift_F12:           
        break;

      /* Ignore Alt keys */
      case K_Alt_KPMinus:         
      case K_Alt_KPPlus:          
      case K_Alt_KPStar:          
      case K_Alt_F2:              
      case K_Alt_F3:              
      case K_Alt_F4:              
      case K_Alt_F5:              
      case K_Alt_F6:              
      case K_Alt_F7:              
      case K_Alt_F8:              
      case K_Alt_F9:              
      case K_Alt_F10:             
      case K_Alt_F11:             
      case K_Alt_F12:             
      case K_Alt_1:               
      case K_Alt_2:               
      case K_Alt_3:               
      case K_Alt_4:               
      case K_Alt_5:               
      case K_Alt_6:               
      case K_Alt_7:               
      case K_Alt_8:               
      case K_Alt_9:               
      case K_Alt_0:               
      case K_Alt_Dash:            
      case K_Alt_Equals:          
      case K_Alt_EHome:           
      case K_Alt_EUp:             
      case K_Alt_EPageUp:         
      case K_Alt_ELeft:           
      case K_Alt_ERight:          
      case K_Alt_EEnd:            
      case K_Alt_EDown:           
      case K_Alt_EPageDown:       
      case K_Alt_EInsert:         
      case K_Alt_EDelete:         
      case K_Alt_KPSlash:         
      case K_Alt_Tab:             
      case K_Alt_Enter:           
      case K_Alt_Escape:          
      case K_Alt_Backspace:       
      case K_Alt_Q:               
      case K_Alt_W:               
      case K_Alt_E:               
      case K_Alt_R:               
      case K_Alt_T:               
      case K_Alt_Y:               
      case K_Alt_U:               
      case K_Alt_I:               
      case K_Alt_O:               
      case K_Alt_P:               
      case K_Alt_LBracket:        
      case K_Alt_RBracket:        
      case K_Alt_Return:          
      case K_Alt_A:               
      case K_Alt_S:               
      case K_Alt_D:               
      case K_Alt_F:               
      case K_Alt_G:               
      case K_Alt_H:               
      case K_Alt_J:               
      case K_Alt_K:               
      case K_Alt_L:               
      case K_Alt_Semicolon:       
      case K_Alt_Quote:           
      case K_Alt_Backquote:       
      case K_Alt_Backslash:       
      case K_Alt_Z:               
      case K_Alt_X:               
      case K_Alt_C:               
      case K_Alt_V:               
      case K_Alt_B:               
      case K_Alt_N:               
      case K_Alt_M:               
      case K_Alt_Comma:           
      case K_Alt_Period:          
      case K_Alt_Slash:          
        break;

      case K_Control_X:           
        return;
        break;
      case K_Control_R:           
        do_redraw_screen();
        break;

      case K_Control_A:           
#ifdef DEBUG
        ScreenClear();
        ScreenSetCursor(0,0);
        for(i = START_ROW; i <= max_row; i++)
        {
          printf("%s%2d:%-3d\033[0m    ",
                 (((i >= top_row) && (i<=bot_row)) ? "\033[35m" : ""),
                 i-START_ROW, cols[i]);
          if ((i-START_ROW)%5 == 4)
            printf("\n");
        }
        printf("\ntop_row = %d(%d),   bot_row = %d(%d)\n",
               top_row, (top_row-START_ROW), bot_row, (bot_row-START_ROW));
        getxkey();
        ScreenClear();
        do_draw_header();
        do_redraw_screen();
#endif
        break;

      /* Help keys */
      case K_Control_F1:          
      case K_Alt_F1:              
      case K_Shift_F1:            
      case K_F1:                  
        help_screen();
        break;

      /* ABORT the editing.  We set the aborted variable and return NULL */
      case K_Control_Q:           
        ScreenSetCursor(8, 6);
        printf(" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= \n");
        ScreenSetCursor(9, 6);
        printf(" |      Are you sure you want to abort [Y]es/[N]o ?      | \n");
        ScreenSetCursor(10, 6);
        printf(" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= \n");
        ScreenSetCursor(9, 58);
        i = getxkey();
        ScreenClear();
        do_draw_header();
        do_redraw_screen();
        if ((i == 'Y') || (i == 'y'))
        {
          aborted = 1;
          return;
        }
        break;
        
      /* Lets just beep at control chars except control X */
      case K_Control_ELeft:       
      case K_Control_ERight:      
      case K_Control_EEnd:        
      case K_Control_EPageDown:   
      case K_Control_EHome:
      case K_Control_EPageUp:      
      case K_Control_EUp:
      case K_Control_EDown:
      case K_Control_EInsert:
      case K_Control_EDelete:   
      case K_Control_B:           
      case K_Control_C:           
      case K_Control_D:           
      case K_Control_E:           
      case K_Control_F:           
      case K_Control_G:           
      case K_Control_K:           
      case K_Control_L:           
      case K_Control_N:           
      case K_Control_O:           
      case K_Control_P:           
      case K_Control_S:           
      case K_Control_T:           
      case K_Control_U:           
      case K_Control_V:           
      case K_Control_W:           
      case K_Control_Y:           
      case K_Control_Z:           
      case K_Control_BackSlash:   
      case K_Control_RBracket:    
      case K_Control_Caret:       
      case K_Control_Underscore:  
      case K_Control_At:          
      case K_Control_Backspace:   
      case K_Control_F2:          
      case K_Control_F3:          
      case K_Control_F4:          
      case K_Control_F5:          
      case K_Control_F6:          
      case K_Control_F7:          
      case K_Control_F8:          
      case K_Control_F9:          
      case K_Control_F10:         
      case K_Control_Left:        
      case K_Control_Right:       
      case K_Control_End:         
      case K_Control_PageDown:    
      case K_Control_Home:        
      case K_Control_PageUp:      
      case K_Control_F11:         
      case K_Control_F12:         
      case K_Control_Up:          
      case K_Control_KPDash:      
      case K_Control_Center:      
      case K_Control_KPPlus:      
      case K_Control_Down:        
      case K_Control_Insert:      
      case K_Control_Delete:      
      case K_Control_KPSlash:     
      case K_Control_KPStar:      
        break;
  
      case K_LineFeed:            
      case K_Return:              
        if (col <= cols[row])
        {
          insert_line(row+1);
          /* Copy end of line to start of new one */
          for (i = col; buffer[row][i] != '\0'; i++)
          {
            buffer[row+1][i-col] = buffer[row][i];
            buffer[row][i] = '\0';
          }
          buffer[row][col] = '\n';
          cols[row] = col;
          cols[row+1] = i-col;
        }
        else
        {
          buffer[row][col] = '\n';
          buffer[row][col+1] = '\0';
          cols[row] = col;
        }
        row++;
        if (row >= bot_row)
        {
          top_row++;
        }
        if (row > max_row)
          max_row = row;
        col = 0;
        do_redraw_screen();
        break;
      
      /* Lets display these ones and store them in the buffer */
      case K_ExclamationPoint:    
      case K_DoubleQuote:         
      case K_Hash:                
      case K_Dollar:              
      case K_Percent:             
      case K_Ampersand:           
      case K_Quote:               
      case K_LParen:              
      case K_RParen:              
      case K_Star:                
      case K_Plua:                
      case K_Comma:               
      case K_Dash:                
      case K_Period:              
      case K_Slash:               
      case K_Colon:               
      case K_SemiColon:           
      case K_LAngle:              
      case K_Equals:              
      case K_RAngle:              
      case K_QuestionMark:        
      case K_At:                  
      case K_LBracket:            
      case K_BackSlash:           
      case K_RBracket:            
      case K_Caret:               
      case K_UnderScore:          
      case K_BackQuote:           
      case K_LBrace:              
      case K_Pipe:                
      case K_RBrace:              
      case K_Tilde:               
      case K_Space:               
      default:
        ScreenPutChar(c, 7, col, SCREEN(row));

        /*  Move the characters already on the line one space left */
        if (col <= cols[row])
        {
          int i;

          for (i = cols[row]; i >= col; i--)
          {
            buffer[row][i+1] = buffer[row][i];
            if (buffer[row][i+1] != '\n')
              ScreenPutChar(buffer[row][i+1], 7, i+1, SCREEN(row));
          }
          cols[row] += 1;
        }

        buffer[row][col] = (char)c;
        col++;
        if (col >= 78)
        {
          if ((in_word == 1) && (word_wrap != 0) && (start_word >= 20))
          {
            cols[row] = start_word;
            insert_line(row+1);
            if (row >= bot_row)
            {
              top_row++;
              do_redraw_screen();
            }
            for (i = start_word; (i >= 0) && (i < col); i++)
            {
              buffer[row+1][i-start_word] = buffer[row][i];
              ScreenPutChar(' ', 7, i, SCREEN(row));
              ScreenPutChar((int)buffer[row][i], 7, i-start_word,SCREEN(row+1));
              buffer[row][i] = ' ';
            }
            buffer[row][start_word] = '\n';
            buffer[row][start_word+1] = '\0';
            col = i - start_word;
            start_word = -1;
            cols[row+1] = col;
          }
          else
          {
            buffer[row][col] = '\n';
            buffer[row][col+1] = '\0';
            cols[row] = col-1;
            col = 0;
          }
          row++;
          if (row > max_row)
            max_row = row;
          if (row >= bot_row)
          {
            top_row++;
            do_redraw_screen();
          }
        }
        break;
    }
  }
}

static void do_redraw_screen()
{
  int i, j;

  ScreenSetCursor(START_ROW, 0);
  for (i = top_row; (i <= bot_row); i++)
  {
    for (j = 0; j < 80; j++)
    {
      int y = START_ROW+i-top_row;

      if ((cols[i] > j) && (buffer[i][j] != '\n'))
        ScreenPutChar(buffer[i][j], 7, j, y);
      else
       ScreenPutChar(' ', 7, j, y);
    }
  }
  ScreenSetCursor(SCREEN(row), col);
  return; 
}

static void do_draw_header()
{
  ScreenSetCursor(0,0);
  printf("%s\n", header);
  printf("MOVE: Arrow keys.  Finish: ^X  Help:F1     Max Row:      Row:   "
         "Col:   \n"
         "=-=-=-=-=-"
         "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n"
        );
  show_info();
  ScreenSetCursor(SCREEN(row), col);
  return;
}

/*
**  show_info() displays the max_row, row, col variables etc
*/
static void show_info()
{
  int r,c;

  ScreenGetCursor(&r, &c);
  ScreenSetCursor(1, 54);
  printf("%-2d", max_row-START_ROW);
  fflush(stdout);
  ScreenSetCursor(1, 61);
  printf("%-2d", row-START_ROW);
  fflush(stdout);
  ScreenSetCursor(1, 68);
  printf("%-2d", col);
  fflush(stdout);
  ScreenSetCursor(r, c);
}

void init_buffer()
{
  int r;

  for (r = 0; r < BUFFER_LINES; r++)
  {
    bzero((char *)buffer[r], LINELEN);
    buffer[r][0] = '\0';
    cols[r] = 0;
  }
}

/*
**  string2buffer takes the string to be edited and turns it into
**  a buffer type.
*/
void string2buffer(char *s)
{
  int r,c;
  char *p;
  int wd = -1;

  for (p = s, r = START_ROW, c = 0; *p != '\0'; p++)
  {
    buffer[r][c] = *p;

    if (*p == '\n')
    {
      cols[r] = c;
      c = 0;
      r++;
      wd = -1;
    }
    else if (isspace(*p))
    {
      wd = -1;
      c++;
    }
    else if ((wd == -1) && (isalnum(*p)))
    {
      wd = c;
      c++;
    }
    else if (c >= 78)
    {
      if ((word_wrap != 0) && (wd > 20))
      {
        cols[r] = wd+1;
        r++;
        for (c = 0; c <= 78-wd; c++)
        {
          buffer[r][c] = buffer[r-1][wd+c];
          buffer[r-1][wd+c] = '\0';
        }
        buffer[r-1][wd] = '\n';
      }
      else
      {
        buffer[r][c+1] = '\n';
        buffer[r][c+2] = '\0';
        cols[r] = c+1;
        r++;
        c = 0;
      }
    }
    else
    {
      c++;
    }
  }
  row = r;
  max_row = r;
  col = c;
  cols[r] = c;
  top_row = START_ROW;
}

/*
**  buffer2string takes the edit buffer and turns it into a string
*/
static void buffer2string(char *s)
{
  int r;

  *s = '\0';
  for (r = 0; r <= max_row; r++)
  {
    strcat(s, buffer[r]);
  }
}

static void beep()
{
  return;
}

static void delete_line(int r)
{
  int x, y;

  for (y = r; y <= max_row; y++)
  {
    for (x = 0; x <= 80; x++)
    {
      if (x <= cols[y+1])
        buffer[y][x] = buffer[y+1][x];
      else
        buffer[y][x] = '\0';
    }
    cols[y] = cols[y+1];
  }
  for (x = 0; x < LINELEN; x++)
    buffer[y][x] = '\0';
  cols[y] = 0;
  max_row--;
}

static void insert_line(int r)
{
  int x, y;

  for (y = max_row; y >= r; y--)
  {
    for (x = 0; x <= 80; x++)
    {
      if (x <= cols[y])
        buffer[y+1][x] = buffer[y][x];
      else
        buffer[y+1][x] = '\0';
    }
    cols[y+1] = cols[y];
  }
  for (x = 0; x < LINELEN; x++)
    buffer[r][x] = '\0';
  cols[r] = 0;
  max_row++;
}

void set_header(char *s)
{
  int i;

  if ((s == NULL) || (*s == '\0'))
  {
    header[0] = '\0';
  } 
  else
  {
    for (i = 0; (i < 60) && (s[i] != '\n') && (s[i] != '\0'); i++)
      header[i] = s[i];
    header[i] = '\0';
  }
}

static void help_screen()
{
  ScreenClear();
  ScreenSetCursor(0, 0);
  printf("  Edit String by: James Willie.  Copyright (C) 1994\n\n");
  printf("^X  <-- means press control and X together\n\n");
  printf("Keys		Function\n--------------------------\n");
  printf("^X		Quit and return string to caller (save)\n");
  printf("^Q		Abort editing.\n");
  printf("^R		Redraw the screen\n");
  printf("F1		This help screen\n");
  printf("Del/Backspace	Delete the current character and move 1 space back\n");
  printf("(Arrow keys)	Move around the screen.\n");

  printf("\n\nPress a key to return to editor ");
  fflush(stdout);
  getxkey();
  
  ScreenClear();
  do_draw_header();
  do_redraw_screen();
  return;
}

int abort_edit()
{
  return ((aborted == 0) ? 0 : 1);
}
