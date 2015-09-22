/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    This file is concerned with loading and writing the locations from/to  *
 *  dirt zone files. At a later stage aber4 files may be included, and       *
 *  since there are several, I will use BUDAPEST file formats.               *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

static char dir[NEXITS] = { 'n', 'e', 's', 'w', 'u', 'd' };

extern int line;

/*
**  Find, load a complete location from file f, creating the location
**  and returning a pointer to it
*/
PLOC get_dirt_loc(f)
	FILE *f;
{
  int c, i, exit_type = X_NORMAL, dir = -1, k;
  char buf[128];
  char *p;
  PLOC l = NULL;
  Bool word = FALSE, direc = FALSE, finished = FALSE;

  if (f == NULL)
    return NULL;

  /* malloc and initialise */
  l = NEW(LOC);
  if (l == NULL)
  {
    printf("Can't create location, Out of memory error.\n");
    exit(1);
  }
  for(i = 0; i < NEXITS; i++)
  {
    l->exit_type[i] = X_NONE;
    l->exits[i] = NULL;
  }
  for(i = 0; i < NUM_LFLAGS; i++)
    l->lflags[i] = FALSE;
  
  /* Although this is not strictly required, enables broken files to be read */
  c = getc(f);
  while (isspace(c))
  {
    if (c == '\n')
      line++;
    c = getc(f);
  }
  ungetc(c,f);
  if (c == '%')
    return NULL;
  
  /* Get location name */
  if (get_word(buf,f) < 0)
    return NULL;
  l->name = COPY(buf);

  /* Get exits */
  i = 0;
  dir = -1;
  finished = FALSE;
  while (!finished && ((c = getc(f)) != EOF))
  {
    if (c == '\n')
      line++;

    if (c == ';')
    {
      c = ' ';
      finished = TRUE;
    }

    if(word)
    {
      if (isspace(c))
      {
        word = FALSE;
        if ((direc) || (dir < 0) || (dir > NEXITS))
        {
          printf("Error on line %d: Location %s, don't understand direction\n",
                 line, l->name);
        }
        else
        {
          buf[i] = '\0';
          l->ename[dir] = COPY(buf);
          l->exit_type[dir] = exit_type;
        }
        dir = -1;
        direc = FALSE;
        i = 0;
      }
      else if (direc)
      {
        if (c == ':')
        {
          exit_type = X_NORMAL;
          direc = FALSE;
        }
      }
      else
      {
        if (c == '^')
          exit_type = X_OBJECT;
        else
          buf[i++] = (char)c;
      } 
    }
    else
    {
      if (!isspace(c))
      {
        direc = TRUE;
        dir = dir2num((char)c);
        word = TRUE;
      }
    }
  } /* While loop */

  if (c == EOF)
  {
    printf("Error on line %d: Found an unexpected End of File.\n", line);
    exit(1);
  }
  NEWLINE(f);
  line++;
  
  /* lflags */
  i = 0;
  while(isalpha(c = getc(f)))
  {
    buf[i++] = (char)c;
  }
  if (c == EOF)
  {
    printf("Error on line %d: Location %s, Lflags expected, found EOF.\n",
           line, l->name);
    return NULL;
  }
  if (strncasecmp(buf,"LFLAGS",i-1) != 0)
  {
    printf("Error on line %d: Did not find Lflags entry for %s.\n", 
           line, l->name);
    return NULL;
  }
  while(c != '{')
  {
    if (c == EOF)
    {
      printf("Error on line %d: Location %s, Lflags expected, found EOF.\n",
             line, l->name);
      return NULL;
    }
    if (c == '\n')
      line++;
    c = getc(f);
  }
  for (c = getc(f); c != '}'; c = getc(f))
  {
    if (c == '\n')
      line++;

    if (!isalpha(c))
      continue;

    for (p = buf; isalpha(c); c = getc(f))
      *p++ = (char)c;
    ungetc(c,f);
    *p = 0;
    k = lookup(l_flags,buf);
    if ((k >= 0) && (k < NUM_LFLAGS))
      l->lflags[k] = TRUE;
    else
      printf("Error line %d: Unknown Lflag [%s] in location %s.\n",
             line, buf, l->name);
  }

  NEWLINE(f);
  if ((l->pname = get_text(f,'^')) == NULL)
  {
    printf("Error line %d: Location ignored.\n",line);
    return NULL;
  }
  NEWLINE(f);
  line += text_lines + 2;
  if ((l->desc = get_text(f,'^')) == NULL)
  {
    printf("Error line %d: Location ignored.\n",line);
    return NULL;
  }
  NEWLINE(f); 
  line += text_lines + 1;
  return l;
}

/*
**  Return the index into dir[] or -1
*/
int dir2num(s)
	char s;
{
  int i = 0;
  
  for (i = 0; i < NEXITS; i++)
    if (dir[i] == s)
      return i;
  return -1;
}

/*
**  Write out the location in dirt format
**  Try to avoid use Xprintf() as it takes up room
*/
void write_dirt_loc(l,f)
	PLOC l;
	FILE *f;
{
  int i;

  if (f == NULL)
    f = stdout;

  fprintf(f,"%s", l->name);
  for(i = 0; i < NEXITS; i++)
  {
    if (l->exit_type[i] != X_NONE)
    {
      fprintf(f, " %c:", dir[i]);
      if (l->exits[i] == NULL)
      {
        if (l->exit_type[i] == X_OBJECT)
        {
          fprintf(stderr,"\nObject not properly defined:%s [Exit in room %s]",
                  l->ename[i], l->name);
          fputc('^',f);
        }
        else 
          fprintf(stderr,"\nLocation not properly defined:%s [Exit in room %s]",
                  l->ename[i], l->name);
        fprintf(f, "%s", l->ename[i]);
      }
      else
      {
        if (l->exit_type[i] == X_OBJECT)
          fputc('^',f);
        fprintf(f, "%s", ((PLOC)l->exits[i])->name);
      }       
    }
  }
  fprintf(f, " ;\nLflags {");
  for(i = 0; i < NUM_LFLAGS; i++)
  {
    if (l->lflags[i] == TRUE)
    {
      fprintf(f, " %s", l_flags[i]);
    }
  }
  fprintf(f, " }\n%s^\n", l->pname);
  write_string(l->desc,f);
  fprintf(f, "^\n\n");
  
  return;
}

/*
**  load_dirt_locs() Continues to read from f, until it finds no locations,
**  either due to EOF(normal) or screwed up input file
*/
PLOC load_dirt_locs(f)
	FILE *f;
{
  PLOC l1, l2;

#ifdef DEBUG
  int x = line;
#endif

  if (f == NULL)
    return NULL;

  l1 = get_dirt_loc(f);
  l2 = l1;

  while(l2 != NULL)
  {
    numloc++;

#ifdef DEBUG
    printf("Location %s: Lines %d to %d\n", l2->name, x, line);
    x = line+1;
#endif

    l2->next = get_dirt_loc(f);
    l2 = l2->next;
  } 
  return l1;
}
