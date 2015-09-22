/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    Handle the input and output of mobile data. Input is driven by io.c    *
 *  while output to stdout can come from mob.c, and to a file from io.c      *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

#ifdef ANSI_C
static void flg_read(int  n, PMOB m, FILE *f);
#else
static void flg_read(n,m,f);
#endif

extern int line;

char *Mob_tab[] = {
    "Name",      "Pname",     "Location",   "Description",
    "End",       "Strength",  "Damage",     "Aggression",
    "Armor",     "Speed",     "Pflags",     "Sflags",
    "Mflags",    "Examine",
    TABLE_END
};

/*
**  The guts of the input routine for mobiles. Reads things a line at a time
**  deciding what each line is, and the reading the rest, putting it back
**  as appropriate. It returns NULL to a driving program if a control % is
**  found, or an error is found.
*/
PMOB read_dirt_mob(f)
	FILE *f;
{
  PMOB m;
  char s[128], n[128];
  int c, d, k, i, x;
  char *t;

  m = NULL;
  for (;;) 
  {
    /* Skip blanks */
    for(c = getc(f); isspace(c); c = getc(f))
    {
      if (c == '\n')
        line++;
    }

    for (t = s; isalnum(c) || c == '_';c = getc(f))
      *t++ = c;
    *t = 0;
    if (t == s) 
    {
      if (c == EOF) 
        return FALSE;
      if (c != '%')
        continue;
      ungetc(c,f);
      return NULL;
      break;
    }
    i = -1;
    while (c == ' ' || c == '\t' || c == '=')
      c = getc(f);
    ungetc(c,f);
    k = lookup(Mob_tab, s);

#ifdef DEBUG
    printf("%s[%4d]\n",s,k);
#endif

    if (k != MOB_NAME && m == NULL) 
    {
      printf("Error on line %d: Entry %s is out of sequence.\n", line, s);
      exit(1);
    }
    switch (k) 
    {
      case MOB_NAME:
        d = 0;
        c = getc(f);
        for (t = n; isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        ungetc(c,f);
        if ((m = NEW(MOB)) == NULL)
        {
          printf("Can't create mobile, Out of memory error\n");
          exit(1);
        }
        init_mob(m,n);
        break;

      case MOB_PNAME:
        if (c == '"')
        {
          c = getc(f);
          m->pname = get_text(f, '"');
          line += text_lines;
        }
        else
        {
          get_word(n,f);
          free(m->pname);
          m->pname = COPY(n);
        }
        break;

      case MOB_LOC:
        for (t = n, c = getc(f); isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
	ungetc(c, f);
        m->locname = COPY(n);
        m->loc = flbname(n);
        break;

      case MOB_DESC:
        /* Hopefully skip any leading new line chars etc. */
        for(c = getc(f); (isspace(c) || (c == '"')); c = getc(f))
        {
          if (c == '\n')
            line++;
        }
        ungetc(c,f);
        m->desc = get_text(f, '"');
        line += text_lines;
        break;

      case MOB_END:
        d = 0;
        c = getc(f);
        for (t = n; isalnum(c) || c == '_'; c = getc(f)) 
          *t++ = c;
        *t = 0;
        ungetc(c,f);
        if (!EQ(m->name,n))
        {
          printf("Error on line %d: Wrong name on end for mob %s\n",
                 line, m->name);
        }
        return m;
        break;

      case MOB_STR:
        m->hp = get_int(f, &x);
        gint_error(x, Mob_tab[x], m->name);
        break;
      case MOB_ARMOR:
        m->armor = get_int(f, &x);
        gint_error(x, Mob_tab[x], m->name);
        break;
      case MOB_DAM:
        m->damage = get_int(f, &x);
        gint_error(x, Mob_tab[x], m->name);
        break;
      case MOB_AGG:
        m->agg = get_int(f, &x);
        gint_error(x, Mob_tab[x], m->name);
        break;
      case MOB_SPEED:
        m->speed = get_int(f, &x);
        gint_error(x, Mob_tab[x], m->name);
        break;
      case MOB_EXAM:
        /* Get the next char, a " and then read till we find 1 \n, or a char */
        getc(f);
        while(((c = getc(f)) != EOF) && isspace(c) && (c != '\n'))
          ;
        if (c == '\n')
          line++;
        else
          ungetc(c,f);
        m->exam  = get_text(f, '"');
        line += text_lines;
        break;
      case MOB_SFLAGS:
        flg_read(1,m,f);
        break;
      case MOB_PFLAGS:
        flg_read(3,m,f);
        break;
      case MOB_MFLAGS:
        flg_read(2,m,f);
        break;
      default:
        fprintf(stderr,"\nError on line %d: Unknown entry [%s]\n", line, s);
        exit(1);
    }
    while ((c = getc(f)) != '\n') 
    {
      if (c == EOF)
        return FALSE;
    }
    line++;
  }
  return NULL;
}

/*
**  Writes a single mobile in dirt format to file f
*/
void write_dirt_mob(m,f)
	PMOB m;
	FILE *f;
{
  int i;
  Bool p = FALSE, s = FALSE, b = FALSE;

  if ((m == NULL) || (m == me))
  {
    printf("That mobile has no statistics\n");
    return;
  }
  fprintf(f,"Name          = %s\n", m->name);
  fprintf(f,"Pname         = \"%s\"\n", m->pname);
  fprintf(f,"Location      = %s\n",
          (m->loc == NULL) ? m->locname : m->loc->name);
  fprintf(f,"Strength      = %d\n", m->hp);
  fprintf(f,"Damage        = %d\n", m->damage);
  fprintf(f,"Armor         = %d\n", m->armor);
  fprintf(f,"Speed         = %d\n", m->speed);
  fprintf(f,"Aggression    = %d\n", m->agg);

  /* Decide if we need to write any of slfags, pflags or mflags */
  for (i = 0; (i < NUM_PFLAGS) || (i < NUM_MFLAGS) || (i < NUM_SFLAGS); i++)
  {
    if ((i < NUM_PFLAGS) && (m->pflags[i] == TRUE))
      p = TRUE;
    if ((i < NUM_MFLAGS) && (m->mflags[i] == TRUE))
      b = TRUE;
    if ((i < NUM_SFLAGS) && (m->sflags[i] == TRUE))
      s = TRUE;
  }
  if (p == TRUE)
  {
    fprintf(f,"Pflags        {");
    for (i = 0; i < NUM_PFLAGS; i++)
      if ((m->pflags[i] == TRUE) && (!EMPTY(p_flags[i])))
        fprintf(f," %s", p_flags[i]);
    fprintf(f," }\n");
  }
  if (b == TRUE)
  {
    fprintf(f,"Mflags        {");
    for (i = 0; i < NUM_MFLAGS; i++)
      if ((m->mflags[i] == TRUE) && (!EMPTY(m_flags[i])))
        fprintf(f," %s", m_flags[i]);
    fprintf(f," }\n");
  }
  if (s == TRUE)
  {
    fprintf(f,"Sflags        {");
    for (i = 0; i < NUM_SFLAGS; i++)
      if ((m->sflags[i] == TRUE) && (!EMPTY(s_flags[i])))
        fprintf(f," %s", s_flags[i]);
    fprintf(f," }\n");
  }
    
  if (!EMPTY(m->desc))
  {
    fprintf(f, "Description   = \"");
    if (strlen(m->desc) > 60)
      fprintf(f, "\n");
    write_string(m->desc,f);
    fprintf(f, "\"\n");
  }
  if (!EMPTY(m->exam))
  {
    fprintf(f, "Examine       = \"\n");
    write_string(m->exam,f);
    fprintf(f, "\"\n");
  }
  fprintf(f,"End           = %s\n\n", m->name);
  return;
}

/*
**  Drives read_dirt_mob() to read all the mobiles in certain section of a file
*/
PMOB read_dirt_mobiles(f)
	FILE *f;
{
  PMOB m1 = NULL, m2 = NULL;

#ifdef DEBUG
  int x = line;
#endif

  m1 = read_dirt_mob(f);
  m2 = m1;
  while(m2 != NULL)
  {
    nummob++;

#ifdef DEBUG
    printf("Mobile %s: Lines %d to %d\n", m2->name, x, line);
    x = line+1;
#endif

    m2->next = read_dirt_mob(f);
    m2 = m2->next;
  }
  return m1;
}

/*
**  Called by read_dirt_mob, to read a set of flags and set appropriate
**  mflags, sflags or pflags.
*/
static void flg_read(n,m,f)
	int  n;
        PMOB m;
	FILE *f;
{
  int j, c, Num;
  char r[128];
  char *t;
  char **tab;
  Bool *flg;

  switch(n)
  {
    case 1:
      tab = s_flags;
      Num = NUM_SFLAGS;
      flg = m->sflags;
      break;
    case 2:
      tab = m_flags;
      Num = NUM_MFLAGS;
      flg = m->mflags;
      break;
    case 3:
      tab = p_flags;
      Num = NUM_PFLAGS;
      flg = m->pflags;
      break;
    default:
      printf("Error, unknown flags\n");
      return;
  }

  for (c = getc(f); c != '}'; c = getc(f))
  {
    if (c == '\n')
      line++;

    if (!isalpha(c))
      continue;
    for (t = r; isalpha(c); c = getc(f))
      *t++ = (char)c;
    ungetc(c, f);
    *t = 0;
    j = lookup(tab,r);
    if ((j >= 0) && (j < Num))
    {
      flg[j] = TRUE;
    }
    else
      printf("Error on line %d: Unknown flag on mobile %s:%s\n", line,
             m->name, r);
  }
  return;
}

void init_mob(m,n)
	PMOB m;
	char *n;
{
  int j;

  m->name = COPY(n);
  m->pname = COPY(n);
  m->level = -1;
  m->locname = NULL;
  m->loc = NULL;
  m->hp = 0;
  m->armor = 0;
  m->damage = 10;
  m->agg = 0;
  m->speed = 5;
  for(j = 0; j < NUM_SFLAGS; j++)
    m->sflags[j] = FALSE;
  for(j = 0; j < NUM_MFLAGS; j++)
    m->mflags[j] = FALSE;
  for(j = 0; j < NUM_PFLAGS; j++)
    m->pflags[j] = FALSE;
  m->desc = NULL;
  m->exam = NULL;
  m->next = NULL;
  return;
}
