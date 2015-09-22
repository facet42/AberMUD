/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 * The IO from loc_io.c, obj_io.c and mob_io.c are just linked together here *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

int line = 1;                /* Counts current line number for error reports */

extern char *glob_table[];

/*
**  save data in dirt format
*/
void save_dirt()
{
  FILE *f;
  char buf[30];
  POBJ o;
  PLOC l;
  PMOB m;

  if (EMPTY(arg[0]))
    strcpy(buf,fzn);
  else
    strcpy(buf,arg[0]);
  f = fopen(buf, "w");
  if (f == NULL)
  {
    printf("Error opening the save file %s",buf);
    return;
  }
  printf("Saving.\n");
  if (comment != NULL)
  {
    fprintf(f, "/*%s*/\n\n", comment);
  }
  fprintf(f, "#include \"ctypes.h\"\n");
  fprintf(f, "#include \"undef.h\"\n");
  fprintf(f, "#include \"exits.h\"\n");
  fprintf(f, "\n%%mobiles\n\n");
  for (m = mob; m != NULL; m = m->next)
    write_dirt_mob(m,f);
  fprintf(f, "%%objects\n\n");
  for (o = obj; o != NULL; o = o->next)
    write_dirt_obj(f,o);
  fprintf(f, "%%locations\n\n");
  for (l = loc; l != NULL; l = l->next)
    write_dirt_loc(l,f);
  fclose(f);
  modified = FALSE;
  return;
}

/*
**  Reads the controlling parts of the dirt zone file which specify
**  if we have a group of locations, object etc. Also in general ignores
**  anything it doesn't understand, except the very first comment
*/
void load_dirt()
{
  PLOC L, l;
  POBJ O, o;
  PMOB M, m;
  FILE *f;
  char buf[81];
  char *s;

  if (arg[0] == NULL)
    strcpy(buf,fzn);
  else
    strcpy(buf,arg[0]);

  f = fopen(buf,"r");
  if (f == NULL)
  {
    printf("Couldn't open file %s.",buf);
    return;
  }
  
  line = 1;
  while((s = fgets(buf, 81, f)) != NULL)
  {
    /* Take into account IBM's \n\r EOLn */
    if ((strlen(s) < 80) || (s[79] == '\n') || (s[80] == '\n'))
      line ++;
    if (strncasecmp("%LOCATIONS", buf, 10) == 0)
    {
      printf("Locations found on line %d. Reading ...\n", line-1);
      L = load_dirt_locs(f);
      if (L == NULL)
        continue;

      /* Add L to main list of locations */
      if (loc == NULL)
        loc = L;
      else  /* Join the 2 lists */
      {
        l = loc;
        while (l->next != NULL)
          l = l->next;
        l->next = L;
      }
    }
    else if (strncasecmp("%MOBILES", buf, 8) == 0)
    {
      printf("Mobiles found on line %d. Reading ...\n", line-1);
      M = read_dirt_mobiles(f);
      if (M == NULL)
        continue;
      if (mob == NULL)
        mob = M;
      else  /* Join the 2 lists */
      {
        m = mob;
        while (m->next != NULL)
          m = m->next;
        m->next = M;
      }
    }
    else if (strncasecmp("%OBJECTS", buf, 8) == 0)
    {
      printf("Objects found on line %d. Reading ...\n", line-1);
      O = read_dirt_objects(f);
      if (O == NULL)
        continue;
      if (obj == NULL)
        obj = O;
      else  /* Join the 2 lists */
      {
        o = obj;
        while (o->next != NULL)
          o = o->next;
        o->next = O;
      }
    }
    else
    {
#ifndef PCC_286
      s = buf;
      while (isspace(*s))
        s++;
      if ((*s == '/') && (*(s+1) == '*'))
      {
        s += 2;
        read_comment(s, f);
      }
#endif
#ifdef DEBUG
      else
        printf("%s", buf);
#endif
    }
  }
  fclose(f);
  printf("Fixing exits...\n");
  fix_exits();
  printf("Fixing mobiles...\n");
  fix_mobs();
  printf("Fixing objects...\n\n");
  fix_objs();
  return;  
}

/*
**  load_config reads file CONFIG_FILE and sets some booleans accordingly
*/
void load_config()
{
  FILE *f;
  char buf[80];
  int k;

  f = fopen(CONFIG_FILE, "r");
  if (f == NULL)
  {
    printf("Could not open config file %s. Using default values\n", 
           CONFIG_FILE);
    return;
  }
  while (get_word(buf, f) >= 0)
  {
    k = lookup(glob_table, buf);
    buf[0] = '\0';

    switch(k)
    {
      case 0:
        brief = read_truth(f);
        break;
      case 1:
        o_counters = read_truth(f);
        break;
      case 2:  /* New oflags */
        printf("Defining Oflags:");
        new_flags(f, o_flags);
        break;
      case 3: /* Lflags */
        printf("Defining Lflags:");
        new_flags(f, l_flags);
        break;
      case 4: 
        printf("Defining Pflags:");
        new_flags(f, p_flags);
        break;
      case 5:
        printf("Defining Mflags:");
        new_flags(f, m_flags);
        break;
      case 6:
        printf("Defining Sflags:");
        new_flags(f, s_flags);
        break;
      case 7:
        verbose = read_truth(f);
        break;
      case 8:
#ifdef _NO_WFLAGS
        read_truth(f);
#else
        wear_flags = read_truth(f);
#endif
        break;
      case 9:
#ifdef _NO_WFLAGS
        printf("Defining Wflags: (compiled out) skipping\n");
        for (k = getc(f); k != '}'; k = getc(f));
#else
        printf("Defining Wflags:");
        new_flags(f, w_flags);
#endif
        break;
      default:
        printf("Whats that?\n");
    }
    buf[0] = '\0';
    NEWLINE(f);
  }
  fclose(f);
  return;
}

/*
**  save globals will do that
*/
void save_glob()
{
  FILE *f;

  f = fopen(CONFIG_FILE, "w");
  if (f == NULL)
  {
    printf("Could not write config file\n");
    return;
  }
  fprintf(f, "Brief %s \nCounter %s \nVerbose %s \n",
          btos(brief), btos(o_counters), btos(verbose));
#ifndef _NO_WFLAGS
  fprintf(f, "Wear_flags %s\n", btos(wear_flags));
#endif
  fprintf(f, "OFLAGS { ");
  write_config_flags(f, o_flags);

#ifndef _NO_WFLAGS
  fprintf(f, "}\nWFLAGS { ");
  write_config_flags(f, w_flags);
#endif

  fprintf(f, "}\nLFLAGS { ");
  write_config_flags(f, l_flags);
  fprintf(f, "}\nPFLAGS { ");
  write_config_flags(f, p_flags);
  fprintf(f, "}\nMFLAGS { ");
  write_config_flags(f, m_flags);
  fprintf(f, "}\nSFLAGS { ");
  write_config_flags(f, s_flags);
  fprintf(f, "}\n");
  fclose(f);
  return;
}

/*
**  write the table of strings, each line being less than 80 chars, but as
**  many as possible without spliting entries.
*/
void write_config_flags(f, table)
	FILE *f;
	char **table;
{
  int i, j;

  for (i = 0, j = 0; table[i] != TABLE_END; i++)
  {
    if (!EMPTY(table[i]))
    {
      j += strlen(table[i]) + 1;
      if (j > 69)
      {
        fprintf(f, "\n         ");
        j = strlen(table[i]);
      }
      fprintf(f, "%s ", table[i]);
    }
  }
  return;
}
    
Bool read_truth(f)
	FILE *f;
{
  char buf[80];

  buf[0] = '\0';
  get_word(buf, f);
  if (ABREV_EQ("TRUE", buf))
    return TRUE;
  else if (ABREV_EQ("FALSE", buf))
    return FALSE;
  else
    printf("Error, expecting TRUE/FALSE.\n");
  return FALSE;
}

/*
**  Read flags from config and try to add them
*/
void new_flags(f, table)
	FILE *f;
	char **table;
{
  int c;
  char flag[80];
  char *t;

  for (c = getc(f); (c != EOF) && (c != '{'); c = getc(f));
  
  for (c = getc(f); c != '}'; c = getc(f))
  {
    if (c == EOF)
      return;
    if (!isalpha(c))
      continue;
    for (t = flag; isalpha(c); c = getc(f))
      *t++ = (char)c;
    ungetc(c,f);
    *t = 0;
    create_flag(flag, table);
    putc('.', stdout);
  }
  printf("\n");
  return;
}

/*
**  This function was designed to read the leading comment from the file
**  in such a way as to preserve it (hopefully!)
**  It should be noted that comments in general are NOT supported.
*/
void read_comment(s, f)
	char *s;
	FILE *f;
{
  char *t;
  char *p;
  char *q;
  char buf[82];
  int  z;

  if (!EMPTY(s))
  {
    z = strlen(s);   
    t = COPY(s);
  }
  else
  {
    t = COPY("\n");
    z = strlen(t);
  }
  
  
  while((p = fgets(buf, 81, f)) != NULL)
  {
    line++;

    if (EMPTY(p))
      continue;
    
    q = p;
    while (isspace(*q))
      q++;
    if ((*q == '*') && (*(q+1) == '/'))
    {
      if (p != q)
      {
        z += (p-q);
        *q = '\0';
        q = (char *)realloc(t, z);
        strcat(q, p);
        t = q;
      }
      break;
    }
    
    z += strlen(p);
    q = (char *)realloc(t, z);
    strcat(q, p);
    t = q;
  }
  comment = t;
  return;
}
