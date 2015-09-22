/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    This file handles the IO related to objects. It's main function is     *
 *  really to read and parse objects, done from read_dirt_obj(). It also     *
 *  writes dirt format objects out, write_dirt_obj() which is used for ostat *
 *  command as well.                                                         *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

extern int line;

char *obj_table[] = {
	"Name",        "Pname",       "Location",    "Description",
	"End",         "Altname",     "Oflags",      "Armor",
	"Damage",      "MaxState",    "State",       "Bvalue",	
	"Size",        "Weight",      "Examine",     "Linked",
	"Counter",     "Wflags",      "Wlevel",
	TABLE_END
};

char *olt_table[] = {
	"UNKNOWN",	"IN_ROOM",	"IN_CONTAINER",	"CARRIED_BY",
	"WORN_BY",	"WIELDED_BY",	"BOTH_BY",
	TABLE_END
};

/*
**  The guts of the reading in of objects, read a word from line, switch
**  to the appropriate entry, parse rest of line. Special cases are %
**  which causes return NULL. Note that a line is discarded if the case
**  does not use it all.
*/
POBJ read_dirt_obj(f)
	FILE *f;
{
  OBJ *o;
  char s[128], n[128], r[128];
  int c, d, k, i, j;
  char *t;

  o = NULL;
  for (;;) 
  {
    do 
    {
      c = getc(f);
      if (c == '\n')
        line++;
    } while (isspace(c));

    for (t = s; isalnum(c) || c == '_';c = getc(f))
      *t++ = c;
    *t = 0;
    if (t == s) 
    {
      if (c == EOF) 
        return NULL;

      if (c != '%')
        continue;
      ungetc(c,f);
      return NULL;
    }
    i = -1;
    if (c == '[') 
    {
      c = getc(f);
      for (i = 0; isdigit(c); i = i * 10 + (c - '0'), c = getc(f));
        if (c != ']') 
          printf("Error on line %d: Not number in [..]\n", line);
      if (c == '\n')
        line++;
      c = getc(f);
    }
    while (c == ' ' || c == '\t' || c == '=')
      c = getc(f);
    ungetc(c,f);
    k = lookup(obj_table,s);

#ifdef DEBUG
    fprintf(stderr,"%d:%s\n", k, s);
#endif

    if (k != O_NAME && o == NULL) 
    {
      printf("Error on line %d: Entry is out of sequence [%s]\n",
             line, s);
      exit(1);
    }
    switch (k) 
    {
      case O_NAME:
        d = 0;
        for (t = n, c = getc(f); isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        ungetc(c,f);
        if ((o = NEW(OBJ)) == NULL)
        {
          printf("Can't create object, Out of memory error.\n");
          exit(1);
        }
        init_obj(n,o);
        numobj++;
        objnum++;
        break;

      case O_PNAME:
        if (c == '"')
        {
          getc(f); /* Skip the " mark then get_text to next one. */
          o->pname = get_text(f,'"');
          line += text_lines;
          break;
        }
        get_word(s,f);
        free(o->pname);
        o->pname = COPY(s);
        break;

      case O_LOC:
        c = getc(f);
        for (t = n; isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        if (c != ':')
        {
          printf("Error on line %d: Object[%s] location is missing :\n",
                 line, o->name);
          exit(1);
        }
        for (t = r, c = getc(f); isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        o->lname = COPY(r);
        o->ltype = lookup(olt_table,n);
        fix_obj(o);
        break;

      case O_DESC:
        if ((i < 0) || (i >= MAX_STATES)) 
        {
          printf("Error on line %d: Illegal index to a desc[]\n", line);
          exit(1);
        }
        getc(f); /* Skip the " */
        while(((c = getc(f)) != EOF) && (isspace(c)) && (c != '\n'))
          ;
        if (!isspace(c))  /* if we didn't skip a \n, put the char back */
          ungetc(c,f);
        o->desc[i] = get_text(f,'"');
        line += text_lines;
        break;

      case O_END:
        d = 0;
        c = getc(f);
        for (t = n; isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        ungetc(c,f);
        if (!EQ(n,o->name))
        {
          printf("Error on line %d: Wrong name to end object.\n", line);
          printf("                  Expected %s found %s\n", o->name, n);
          exit(1);
        }
        return o;
        break;

      case O_ANAME:
        if (c == '"')
        {
          getc(f); /* Skip the " mark then get_text to next one. */
          o->altname = get_text(f, '"');
          line += text_lines;
          break;
        }
        get_word(s,f);
        o->altname = COPY(s);
        break;

      case O_WFLAGS:
#ifdef _NO_WFLAGS
        for (c = getc(f); c != '}'; c = getc(f))
        {
          if (c == '\n')
            line++;
        }
#else
        if (!wear_flags)
        {
          printf("Error in %s: Wflags aren't turned on. They won't save\n",
                 o->name);
        }
        while (c != '{')
        {
          if (c == '\n')
            line++;
          if (c == EOF)
          {
            printf("Error on line %d: Object %s EOF found, expected Wflags\n",
                   line, o->name);
            return NULL;
          }
          c = getc(f);
        }
        for (c = getc(f); c != '}'; c = getc(f))
        {
          if (c == '\n')
            line++;

          if (!isalpha(c))
            continue;

          for (t = r; isalpha(c); c = getc(f))
            *t++ = (char)c;
          ungetc(c,f);
          *t = 0;
          j = lookup(w_flags, r);
          if ((j >= 0) && (j < NUM_WFLAGS))
            o->wflags[j] = TRUE;
          else
            printf("Error on line %d: Unknown wflag %s in object %s\n",
                   line, r, o->name);
        }
#endif

        break;

      case O_OFLAGS:
        while (c != '{')
        {
          if (c == '\n')
            line++;
          if (c == EOF)
          {
            printf("Error on line %d: Object %s EOF found, expected Oflags\n",
                   line, o->name);
            return NULL;
          }
          c = getc(f);
        }
        for (c = getc(f); c != '}'; c = getc(f))
        {
          if (c == '\n')
            line++;

          if (!isalpha(c))
            continue;

          for (t = r; isalpha(c); c = getc(f))
            *t++ = (char)c;
          ungetc(c,f);
          *t = 0;
          j = lookup(o_flags, r);
          if ((j >= 0) && (j < NUM_OFLAGS))
            o->oflags[j] = TRUE;
          else
            printf("Error on line %d: Unknown oflag %s in object %s\n",
                   line, r, o->name);
        }
        break;

      case O_ARMOR:
        o->armor = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;

      case O_COUNT:
        if (!o_counters)
         printf("Warning: a counter entry found, and o_counters not allowed\n");
        o->counter = get_int(f, &j);
        gint_error(j,obj_table[k],o->name);
        break;

      case O_DAMAGE:
        o->damage = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_WLEVEL:
#ifdef _NO_WFLAGS
        get_int(f, &i);
#else
        o->wlevel = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
#endif
        break;
      case O_MAXS:
        o->maxstate = get_int(f, &j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_STATE:
        o->state = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_VALUE:
        o->value = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_SIZE:
        o->size = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_WEIGHT:
        o->weight = get_int(f,&j);
        gint_error(j,obj_table[k],o->name);
        break;
      case O_LINK:
        get_word(r,f);
        o->link = COPY(r); 
        break;
      case O_EXAM:
        getc(f);
        while(((c = getc(f)) != EOF) && isspace(c) && (c != '\n'))
          ;
        if (c == '\n')
          line++;
        else
          ungetc(c,f);
        o->examine = get_text(f,'"');
        line += text_lines;
        break;
      default:
        printf("Error on line %d: unknown entry [%s]\n", line, s);
        exit(1);

    }
    while (c != '\n') 
    {
      if (c == EOF)
        return NULL;
      c = getc(f);
    }
    line++;
  }
  printf("Eeeeeeek!!! Error of some sort\n");
  return NULL;
}

/*
**  read_dirt_objects, does exactly that, uses read_dort_obj to read
**  sequence of 0 or more objects
*/
POBJ read_dirt_objects(f)
	FILE *f;
{
  POBJ o1 = NULL, o2 = NULL;

#ifdef DEBUG
  int x = line;
#endif

  o1 = read_dirt_obj(f);
  o2 = o1;

  while(o1 != NULL)
  {
#ifdef DEBUG
    printf("Object %s: Lines %d to %d\n", o1->name, x, line);
    x = line+1;
#endif

    o1->next = read_dirt_obj(f);
    o1 = o1->next;
  }
  return o2;
}

/*
**  Write out object o, to file f
*/
void write_dirt_obj(f,o)
	FILE *f;
	POBJ o;
{
  int i;
  Bool test;

  if (o == NULL)
  {
    printf("NULL object\n");
    return;
  }
  fprintf(f,"Name          = %s\n",o->name);
  fprintf(f,"Pname         = \"%s\"\n",o->pname);
  if (o->altname != NULL)
    fprintf(f,"AltName       = \"%s\"\n",o->altname);
  fprintf(f,"Location      = %s:%s\n",olt_table[(int)o->ltype],oloc_name(o));
    
  fprintf(f, "Oflags        { ");
  write_oflags(f,o);
  fprintf(f, "}\n");

#ifndef _NO_WFLAGS
  if (wear_flags == TRUE)
  {
    test = FALSE;
    for (i = 0; (i < NUM_WFLAGS) && (test == FALSE); i++)
    {
      if (o->wflags[i] == TRUE)
        test = TRUE;
    }
    if (test == TRUE)
    {
      fprintf(f, "Wflags        { ");
      write_wflags(f,o);
      fprintf(f, "}\n");
      if (o->wlevel >= 0)
        fprintf(f, "WLevel        = %d\n", o->wlevel);
    }
  }
#endif

  if (o->value != 0)
    fprintf(f,"BValue        = %d\n",o->value);
  if (o->armor != 0)
    fprintf(f,"Armor         = %d\n",o->armor);
  if (o->damage != 0)
    fprintf(f,"Damage        = %d\n",o->damage);
  if ((o_counters) && (o->counter != 0))
    fprintf(f,"Counter       = %d\n", o->counter);
  if (o->size != 0)
    fprintf(f,"Size          = %d\n",o->size);
  if (o->weight != 0)
    fprintf(f,"Weight        = %d\n",o->weight);
  if (o->maxstate > 0)
  {
    fprintf(f,"MaxState      = %d\n",o->maxstate);
    fprintf(f,"State         = %d\n",o->state);
  }
  if (!EMPTY(o->link))
    fprintf(f,"Linked        = %s\n",o->link);
  for (i = 0; i < MAX_STATES; i++)  /* Used to wrongly stop at o->maxstate */
  {                                 /* Thanks to Mary for bug report       */
    if (!EMPTY(o->desc[i]))
    {
      fprintf(f,"Desc[%1d]       = \"",i);
      if (strlen(o->desc[i]) > 60)
        fputc('\n', f);
      fprintf(f,"%s\"\n",o->desc[i]);
    }
  }
  if (!EMPTY(o->examine))
  {
    fprintf(f,"Examine       = \"\n");
    write_string(o->examine,f);
    fprintf(f, "\"\n");
  }
  fprintf(f,"End           = %s\n\n",o->name);
  return; 
}

#ifndef _NO_WFLAGS
void write_wflags(f,o)
	FILE *f;
	POBJ o;
{
  int i;

  for (i = 0; i < NUM_WFLAGS; i++)
  {
    if (o->wflags[i] == TRUE)
      fprintf(f,"%s ", w_flags[i]);
  }
  return;
}
#endif

void write_oflags(f,o)
	FILE *f;
	POBJ o;
{
  int i;

  for (i = 0; i < NUM_OFLAGS; i++)
  {
    if (o->oflags[i] == TRUE)
      fprintf(f,"%s ", o_flags[i]);
  }
  return;
}

/*
**  Initialize the new object 
*/
void init_obj(n,o)
	char *n;
	POBJ o;
{
  int j;

  if (o == NULL)
    return;

  o->name = COPY(n);
  o->number = objnum;
  o->pname = COPY(o->name);
  o->link = NULL;
  o->altname = NULL;
  o->location = NULL;
  o->lname = NULL;
  o->ltype = UNKNOWN;
  o->damage = 0;
  o->armor = 0;
  for(j = 0; j < NUM_OFLAGS; j++)
    o->oflags[j] = FALSE;

#ifndef _NO_WFLAGS
  for(j = 0; j < NUM_WFLAGS; j++)
    o->wflags[j] = FALSE;
  o->wlevel = -1;
#endif

  o->state = 0;
  o->maxstate = 0;
  o->value = 0;
  o->size = 0;
  o->weight = 0;
  o->counter = 0;
  for(j = 0; j < MAX_STATES; j++)
    o->desc[j] = NULL;
  o->examine = NULL;
  return;
}
