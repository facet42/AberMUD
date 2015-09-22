/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   ABERED (AberMUD Dirt3 Zone editor)                      *
 *                                                                           *
 *    This file contains generally useful utility functions as well as the   *
 *  functions from the berkely include file, strncasecmp and strcasecmp.     *
 *  see below for more details. You must define EQBUG to get them.           *
 *                                                                           *
 *          Copyright (C) 1993. James Willie. All rights reserved.           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

/*
**  Try to add the char *flag to the char **table if there is a NULL slot
*/
void create_flag(flg, table)
	char *flg;
	char **table;
{
  int i;

  if (EMPTY(flg))
    return;

  for (i = 0; (table[i] != TABLE_END) && (table[i] != NULL); i++);
  if (table[i] == TABLE_END)
  {
    printf("Can't define flag %s because flag table is full.\n", flg);
    return;
  }
  table[i] = COPY(flg);
  return;
} 

/*
**  List the contents of table (char **table)
*/
void list_flags(table)
	char **table;
{
  int i, j;

  for (i = 0, j = 0; table[i] != TABLE_END; i++)
  {
    if (EMPTY(table[i]))
      continue;
    printf("%-13s  ",table[i]);
    if ((j % 5) == 4)
      printf("\n");
    j++;
  }
  printf("\n");
  return;
}

/*
**  Remove trailing \n's and/or \r's from the string t
*/
void fix_string(t)
	char *t;
{
  int z;

  if (EMPTY(t))
    return;
  z = strlen(t);
  if (z >= 1)
  {
    z--;
    if ((t[z] == '\n') || (t[z] == '\r'))
      t[z] = '\0';
    if (z >= 1)
      z--;
    if ((t[z] == '\n') || (t[z] == '\r'))
      t[z] = '\0';
  }
  return;
}

/*
**  Simple function to look up a string in a table of strings, return the 
**  entry number
*/
int lookup(table,entry)
	char **table;
	char *entry;
{
  int i,x = 0;
  char **u;

  i = strlen(entry);
  if (i <= 0)
    return -1;

  for(u = table, x = 0; *u != TABLE_END; u++, x++)
  {
    if (*u == NULL)
      continue;
    if (strncasecmp(entry, *u, i) == 0)
      return x;
  }
  return -1;
}

/*
**  Sends standard error message for status of get_int()
*/
void gint_error(s,n,o)
	int s;
	char *n;
	char *o;
{
  if (s == 1)
  {
    printf("Error: [%s] Value for %s was missing\n",o,n);
  }
  else if (s > 1)
  {
    printf("Error: [%s] Value for %s was NOT an integer\n",o,n);
  }
  return;
}

/* 
**  Read an integer from file stream f (Can be stdin or actual file)
**  The calling function is responsible for positioning the file
*/
int get_int(F, status)
	FILE *F;
	int *status;
{
  int i, c;
  Bool neg;

  *status = 0;
  c = getc(F);
  while ((c == ' ') || (c == '\t'))
    c = getc(F);
  if (c == '\n')
  {
    *status = 1;
    ungetc(c,F);
    return 0;
  }
  i = 0;
  if ((neg = (c == '-')))
    c = getc(F);

  if (!isdigit(c))
  {
    *status = 2;
    ungetc(c,F);
    return 0;
  }

  while (isdigit(c)) 
  {
    i = i * 10 + (c - '0');
    c = getc(F);
  }
  ungetc(c,F);
  if (neg)
    i = -i;
  return i;
}

/*
**  Read single word into s from file f
*/
int get_word(s,f)
	char *s;
        FILE *f;
{
  int c, i = 0;

  s[0] = '\0';
  while(isspace(c = getc(f)));
  if (c == EOF)
    return -1; 
  
  ungetc(c,f);
  while(!isspace(c = getc(f)) && (c != ';') && (c != EOF))
  {
    s[i++] = (char)c;
  }
  ungetc(c,f); /* put last character back */
  s[i] = '\0';
  return 0;
}

/*
**  Boolean to string (Pretty obvious what it does?)
*/
char *btos(t)
	Bool t;
{
  return (t ? "True " : "False");
} 

/*
**  General routine to read in text, into it's internal buffer, looking 
**  for the character t. Also terminates if either EOF is found, or buffer
**  overflows.
**  If the number of lines read is needed, text_lines will hold the info
*/
char *get_text(f,t)
	FILE *f;
	char t;
{
  char buff[MAX_BUFFER+1];
  int i,c;

  text_lines = 0;

  i = 0;
  while (((c = getc(f)) != EOF) && (c != t))
  {
    if (c == '\n')
      text_lines++;

    if (i > MAX_BUFFER)
    {
      fprintf(stderr,"Buffer overflow error\n");
      break;
    }
    else
      buff[i++] = (char)c;
    
  }
  if (c == EOF)
  {
    fprintf(stderr, "Error : EOF found, expecting [%c].\n",t);
    return NULL;
  }

  /* Special case for IBM and reading a single line, the \r\n we terminated */
  /* on \n, but don't want the \r either. Should really terminate on \r.    */
  if ((t == '\n') && (i >= 1) && (buff[i-1] == '\r'))
    i--;
  buff[i] = '\0';

  return (char *)COPY(buff);
}

/*
**  get_arg will return the next argument from the command line. pnt
**  points to current position.
*/
char *get_arg()
{
  char buf[MAX_COM+1];
  int i = 0;

  if (*pnt == '\0')
    return NULL;

  while(isspace(*pnt))
    pnt++;
  while(!isspace(*pnt) && (*pnt != '\0'))
  {
    buf[i++] = *pnt++;
  }
  buf[i] = '\0';

  if ((i < 1) || (buf[0] == '\0'))
    return NULL;

  return (char *)COPY(buf);
}
  
/*
**  Stupid IBM overflows it buffer when doing printf of long strings, so
**  lets do it for the dumb thing
*/
void write_string(s,f)
	char *s;
	FILE *f;
{
  char *p;

  p = s;
  while(*p != '\0')
  {
    fputc(*p++,f);
  }
}

#ifdef EQBUG
/* ===========NOTE:  I will mark the end of the Berkeley file. ============= */

/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific written prior permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static u_char charmap[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int strcasecmp(s1, s2)
	char *s1, *s2;
{
  register u_char *cm = charmap, *us1 = (u_char *)s1, *us2 = (u_char *)s2;

  while (cm[*us1] == cm[*us2++])
    if (*us1++ == '\0')
      return(0);
    return(cm[*us1] - cm[*--us2]);
}

int strncasecmp(s1, s2, n)
	char *s1, *s2;
        register int n;
{
        register u_char *cm = charmap,
                        *us1 = (u_char *)s1,
                        *us2 = (u_char *)s2;

        while (--n >= 0 && cm[*us1] == cm[*us2++])
                if (*us1++ == '\0')
                        return(0);
        return(n < 0 ? 0 : cm[*us1] - cm[*--us2]);
}

/* ================== End of the Berkely include ========================== */
#endif

