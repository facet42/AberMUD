/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   ABERED (AberMUD Dirt3 Zone editor)                      *
 *                                                                           *
 *    This file handles getting the command line arguments etc. The program  *
 *  takes the zone name as input, and expects it's input and output files    *
 *  to be zonename.zon dor both reading and writing. It does not make a      *
 *  backup copy first, but you can force it to dump to another filename.     *
 *  This also contains general purpose commands, not belonging to loc, ...   *
 *  Usage: abered zone_name                                                  *
 *                                                                           *
 *          Copyright (C) 1993. James Willie. All rights reserved.           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "prototyp.h"
#include "verbs.h"

#define VERSION		"1.3"

/* ============================== Globals ================================= */

PLOC   loc;                       /* Current list of locations.             */
POBJ   obj;                       /* Current list of objects.               */ 
PMOB   mob;                       /* Current list of mobiles.               */
PMOB   me;                        /* Loc of things I pick up                */
int    objnum;                    /* Next objects number                    */
int    numobj;                    /* Total number of objects.               */
int    nummob;                    /* Number of mobiles                      */
int    numloc;                    /* Number of locations.                   */
int    text_lines;                /* Holds number lines read by get_text()  */
FILE   *ferr;                     /* The error file.                        */
char   cmdln[MAX_COM+1];          /* buffer to read the command into.       */
char  *cmd;                       /* Pointer to just the command.           */
char  *arg[3];                    /* Up to 3 buffers for arguments.         */
char  *pnt;                       /* pointer into cmd                       */
char  *comment;                   /* Allow for leading comments at start    */
Bool   quit;                      /* Quit loop, exit loop if quit           */
Bool   modified;                  /* Have we edited anything?               */

PLOC   myloc;                     /* User current location                  */
Bool   brief;                     /* Brief mode for movement                */
Bool   color;                     /* Can user see color. (NOT IMP)          */
Bool   o_counters;                /* We allowing o->counter ?               */
Bool   verbose;                   /* Verbose mode stats thing editing       */

char   zone[128];                 /* Name of the zone. (Also read/write file*/
char   fzn[128];

char   *o_flags[NUM_OFLAGS + 1];  /* Tables of flags                        */
char   *p_flags[NUM_PFLAGS + 1];
char   *m_flags[NUM_MFLAGS + 1];
char   *s_flags[NUM_SFLAGS + 1];
char   *l_flags[NUM_LFLAGS + 1];

#ifndef _NO_WFLAGS
Bool   wear_flags;                /* Are Wflags defined?                    */
char   *w_flags[NUM_WFLAGS + 1];
#endif

char *glob_table[] = {
    "Brief",       "Counters",        "Oflags",        "Lflags",
    "Pflags",      "Mflags",          "Sflags",        "Verbose",
    "Wear_flags",  "Wflags",
    TABLE_END
};

void main(argc, argv)
	int argc;
	char **argv;
{
  int i;
  FILE *f = NULL;

  if (argc < 2)
    strcpy(zone,"zone");
  else
    strcpy(zone,argv[1]);

#ifdef DOS_MACHINE
  for(i = 0; i < 8; i++)     /* IBM file names max length of 8 chars */
#else
  for(i = 0; i < 121; i++)
#endif
  {
    if (zone[i] == '\0')
      break;
    fzn[i] = zone[i];
  }
  fzn[i++] = '.';
  fzn[i++] = 'z';
  fzn[i++] = 'o';
  fzn[i++] = 'n';
#ifndef DOS_MACHINE
  fzn[i++] = 'e';
#endif
  fzn[i] = '\0';

  /* Test that the default file (zone.zone) exists */
  if (argc < 2) 
  {
    if ((f = fopen(fzn, "r")) == NULL)
    {
      usage(2);
    }
    fclose(f);
  }
  usage(0);
  printf("\nZone Name:%s\nFile name:%s\n\n", zone, fzn);

  /* Initialization */
  ferr = stderr;
  loc = NULL;
  obj = NULL;
  mob = NULL;
  me = (PMOB)(-1);
  objnum = 1;
  numobj = 0;
  numloc = 0;
  nummob = 0;
  arg[0] = NULL;
  brief = FALSE;
  o_counters = FALSE;
  modified = FALSE;
  quit = FALSE;
  comment = NULL;

#ifdef IBM_MLJ_EDIT
  set_word_wrap();
#endif

  for (i = 0; i < NUM_OFLAGS; i++)
    o_flags[i] = NULL;
  o_flags[NUM_OFLAGS] = TABLE_END;

#ifndef _NO_WFLAGS
  for (i = 0; i < NUM_WFLAGS; i++)
    w_flags[i] = NULL;
  w_flags[NUM_WFLAGS] = TABLE_END;
  wear_flags = FALSE;
#endif

  for (i = 0; i < NUM_PFLAGS; i++)
    p_flags[i] = NULL;
  p_flags[NUM_PFLAGS] = TABLE_END;

  for (i = 0; i < NUM_MFLAGS; i++)
    m_flags[i] = NULL;
  m_flags[NUM_MFLAGS] = TABLE_END;

  for (i = 0; i < NUM_SFLAGS; i++)
    s_flags[i] = NULL;
  s_flags[NUM_SFLAGS] = TABLE_END;

  for (i = 0; i < NUM_LFLAGS; i++)
    l_flags[i] = NULL;
  l_flags[NUM_LFLAGS] = TABLE_END;

  load_config();
  load_dirt();
  myloc = loc;
  lookin(myloc);

  /* Get and do commands until user is finished */
  while((!quit) || (modified))
  {
    cmdln[0] = '\0';
    pnt = NULL;
    get_command();
    if (EMPTY(cmdln))
      continue;
    game_com(cmdln);

    if((modified) && (quit))
    {
      printf("You have not saved. Really want to quit [NO/yes]?");
      get_command();
      pnt = cmdln;
      cmd = get_arg();
      if((cmd != NULL) && (strncasecmp("Yes",cmd,strlen(cmd)) == 0))
        modified = FALSE;
      else
      {
        free_args();
        quit = FALSE;
      }
    }
  }
  return;
}

/*
**  Free the arg[] variables up for reuse
*/
void free_args()
{
  int i;

  for(i = 0; i <= 2; i++)
  {
    if (arg[i] != NULL)
    {
      free(arg[i]);
      arg[i] = NULL;
    }
  }
  return;
}

/*
**  set_args allows functions to set the 3 arg[] variables as if they had
**  come from a command line.
*/
void set_args(a,b,c)
	char *a;
	char *b;
	char *c;
{
  free_args();
  if (a != NULL)
    arg[0] = COPY(a);
  if (b != NULL)
    arg[2] = COPY(b);
  if (c != NULL)
    arg[3] = COPY(c);
  return;
}

/*
**  Get the next command line. Free the global args, call get_arg for
**  and get the first 3 args of the command line.
*/
void get_command()
{
  int i, c;
  free_args();
  if (cmd != NULL)
  {
    free(cmd);
    cmd = NULL;
  }

  putc('>', stdout);
  i = 0;
  while(((c = getc(stdin)) != '\n'))
  {
    if (i < MAX_COM)
      cmdln[i++] = (char)c;
    else
      putc('\a',stdout);
  }
  cmdln[i] = '\0';
  return;
}

/*
**  Execute a game command. Is called with cmdln, but can be called from
**  anywhere
*/
void game_com(s)
	char *s;
{
  Bool p;

  if (EMPTY(s))
    return;
  p = ((s == cmdln) ? FALSE : TRUE);
  free_args();
  pnt = s;
  cmd = get_arg();
  arg[0] = get_arg();
  arg[1] = get_arg();
  arg[2] = get_arg();
  if (cmd != NULL)
    parse_com(p);
  return;
}

/*
**  Display list of commands. Or display a file on a command
*/
void helpcom(s)
	char *s;
{
  int t, c;
  char fname[80], buf[80];
  FILE *f = NULL;
  
  if (EMPTY(s))
  {
    printf("\tNote that some of the commands might be aliases for each other");
    printf("\n\033[1;33m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    printf("\033[1;33m-=-=-=-=-=-=-=-=-=-=-=-=-=\033[0m");
    for(t = 0; verbs[t] != TABLE_END; t++)
    {
      if ((t % 6) == 0)
        printf("\n    ");
      printf("%-10s ",verbs[t]);
    }
    if ((t % 6) != 0) /* t is 1 larger than last command we wrote */
      printf("\n");

    printf("\033[1;33m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    printf("\033[1;33m-=-=-=-=-=-=-=-=-=-=-=-=-=\033[0m\n");
    return;
  }
  if (ABREV_EQ("QUIT", s))
  {
    printf("YOUR JOKING??? You need help on quit? You'll leave the editor.\n");
    printf("If you have modified anything it will ask if your sure.\n");
    return;
  }
  t = lookup(verbs, s);
  buf[0] = '\0'; 

  switch(t)
  {
    case VERB_COMMENT:
      strcpy(buf, "comments");
      break;
    case VERB_CLONE:
      strcpy(buf, "clone");
      break;
    case VERB_LEDIT:
      strcpy(buf, "ledit");
      break;
    case VERB_OEDIT:
      strcpy(buf, "oedit");
      break;
    case VERB_MEDIT:
      strcpy(buf, "medit");
      break;
    case VERB_GLOBALS:
    case VERB_BRIEF:
    case VERB_SCORE:
    case VERB_DEFINE:
    case VERB_UNDEFINE:
      strcpy(buf, "global");
      break;
    case VERB_SAVE:
    case VERB_DUMP:
      strcpy(buf, "save");
      break;
    case VERB_INVENT:
    case VERB_EXAMINE:
    case VERB_GET:
    case VERB_GIVE:
    case VERB_PUT:
    case VERB_TAKE:
    case VERB_LOOK:
    case VERB_DROP:
      strcpy(buf, "commands");
      break;
    case VERB_HELP:
    case VERB_HELP2:
      strcpy(buf, "general");
      break;
    case VERB_LFLAGS:
    case VERB_OFLAGS:
    case VERB_MFLAGS:
    case VERB_SFLAGS:
    case VERB_PFLAGS:
    case VERB_WFLAGS:
      strcpy(buf, "flags");
      break;
    case VERB_MSTATS:
    case VERB_OSTATS:
    case VERB_LSTATS:
      strcpy(buf, "stats");
      break;
    case VERB_LCREATE:
    case VERB_OCREATE:
    case VERB_MCREATE:
      strcpy(buf, "create");
      break;
    case VERB_LIST:
      strcpy(buf, "list");
      break;
    case VERB_NORTH:
    case VERB_SOUTH:
    case VERB_EAST:
    case VERB_WEST:
    case VERB_DOWN:
    case VERB_UP:
    case VERB_LINK:
    case VERB_EXITS:
    case VERB_GOTO:
    case VERB_TRANS:
    case VERB_SUMMON:
      strcpy(buf, "move");
      break;
    case VERB_CHECK:
      strcpy(buf, "errors");
      break;
    case VERB_MZAP:
    case VERB_LZAP:
    case VERB_OZAP:
    case VERB_KILL:
      strcpy(buf, "zap");
      break;

    default:
      printf("There is no help available on %s. Sorry.\n", s);
      return;
  }
  sprintf(fname, "%s%s.hlp", HELP_DIR, buf);
  f = fopen(fname, "r");
  if (f == NULL)
  {
    printf("Error opening file %s for help on %s\n", fname, s); 
    return;
  }
  printf("\033[2JHELP:%s\n\n",verbs[t]);
  t = 1;
  while ((c = getc(f)) != EOF)
  {
    if (c == '\n')
    {
      if (t >= 20)
      {
        my_pause();
        t = 1;
      }
      else
        t++;
    }
    putc(c, stdout);
  }
  putc('\n', stdout);
  fclose(f); 
  return;
}

/*
**  Determine what command, then call appropriate function
*/
void parse_com(quiet)
	Bool quiet;
{
  int v = -1;

  if (cmd == NULL)
    return;
  putc('\n', stdout);
  if (strcasecmp(cmd,"quit") == 0)
    quit = TRUE;
  else if (strncasecmp("quit",cmd,strlen(cmd)) == 0)
    printf("You must type quit to exit, no more no less\n");
  else
  {
    v = lookup(verbs,cmd);
    switch(v)
    {
      case VERB_NORTH:
      case VERB_SOUTH:
      case VERB_EAST:
      case VERB_WEST:
      case VERB_UP:
      case VERB_DOWN:
        dodirn(v);
        break;
      case VERB_LOOK:
        if (brief)
        {
          brief = FALSE;
          lookin(myloc);
          brief = TRUE;
        }
        else
          lookin(myloc);
        break;
      case VERB_LSTATS:
        rstatscom();
        break;
      case VERB_BRIEF:
        brief = (brief ? FALSE : TRUE);
        break;
      case VERB_COMMENT:
        comments();
        break;
      case VERB_EXITS:
        exitcom();
        break;
      case VERB_GOTO:
        gotocom();
        break;
      case VERB_SAVE:
      case VERB_DUMP:
        save_dirt();
        break;
      case VERB_LEDIT:
        redit_com();
        break;
      case VERB_LFLAGS:
        lflags();
        break;
      case VERB_HELP:
      case VERB_HELP2:
        helpcom(arg[0]);
        break;
      case VERB_WFLAGS:
        wflags();
        break;
      case VERB_LIST:
        if (EMPTY(arg[0]) || (ABREV_EQ("LOCATIONS",arg[0])))
          list_loc();
        else if (ABREV_EQ("OBJECTS",arg[0]))
          list_obj();
        else if (ABREV_EQ("MOBILES",arg[0]))
          list_mob();
        else if (ABREV_EQ("OFLAGS", arg[0]))
        {
          printf("Oflags:\n");
          list_flags(o_flags);
        }
        else if (ABREV_EQ("LFLAGS", arg[0]))
        {
          printf("Lflags:\n");
          list_flags(l_flags);
        }
        else if (ABREV_EQ("PFLAGS", arg[0]))
        {
          printf("Pflags:\n");
          list_flags(p_flags);
        }
        else if (ABREV_EQ("SFLAGS", arg[0]))
        {
          printf("Sflags:\n");
          list_flags(s_flags);
        }
        else if (ABREV_EQ("MFLAGS", arg[0]))
        {
          printf("Mflags:\n");
          list_flags(m_flags);
        }
        else if (ABREV_EQ("WFLAGS", arg[0]))
        {
#ifdef _NO_WFLAGS
          no_wflags();
#else
          printf("Wflags:\n");
          list_flags(w_flags);
#endif
        }
        else
        {
          printf("List what?\n");
          printf("Usage: List [Location/Object/Mobile/<W O L M S P>flags]\n");
        }
        break;
      case VERB_LCREATE:
        create_loc();
        break;
      case VERB_LZAP:
        destroy_loc();
        break;
      case VERB_OSTATS:
        ostat(arg[0]);
        break;
      case VERB_OFLAGS:
        oflags();
        break;
      case VERB_MSTATS:
        mstat(arg[0]);
        break;
      case VERB_MFLAGS:
        mob_flags(2);
        break;
      case VERB_PFLAGS: 
        mob_flags(3);
        break;
      case VERB_SFLAGS:
        mob_flags(1);
        break;
      case VERB_CHECK:
        check();
        break;
      case VERB_MEDIT:
        mset();
        break;
      case VERB_OEDIT:
        oedit();
        break;
      case VERB_GLOBALS:
      case VERB_SCORE:
        globals();
        break;
      case VERB_LINK:
        link_door(arg[0], arg[1]);
        break;
      case VERB_OCREATE:
        ocreate();
        break;
      case VERB_OZAP:
        ozap(arg[0]);
        break;
      case VERB_INVENT:
        inventory();
        break;
      case VERB_SUMMON:
      case VERB_TRANS:
        summon(arg[0]);
        break;
      case VERB_MCREATE:
        mcreate(arg[0]);
        break;
      case VERB_EXAMINE:
        examine(arg[0]);
        break;
      case VERB_DROP:
        drop(arg[0]);
        break;
      case VERB_DEFINE:
        define(arg[1], arg[0]);
        break;
      case VERB_UNDEFINE:
        if (EMPTY(arg[0]) || EMPTY(arg[1]))
        {
          printf("undefine Xflags flagname\n");
          break;
        }
        undefine(arg[1], arg[0]);
        break;
      case VERB_GET:
      case VERB_TAKE:
        get(arg[0]);
        break;
      case VERB_PUT:
        put(arg[0], arg[1], arg[2]);
        break;
      case VERB_GIVE:
        give(arg[0], arg[1]);
        break;
      case VERB_MZAP:
        mzap(arg[0], TRUE);
        break;
      case VERB_KILL:
        mzap(arg[0], FALSE);
        break;

      case VERB_CLONE:
        if (EMPTY(arg[0]) || EMPTY(arg[1]) || EMPTY(arg[2]))
        {
          printf("clone <LOC|MOB|OBJ> <existing> <new name>\n");
          return;
        }
        else if (ABREV_EQ("LOCATION", arg[0]))
        {
          clone_loc(arg[1], arg[2]);
          return;
        }
        else if (ABREV_EQ("MOBILE", arg[0]))
        {
          clone_mob(arg[1], arg[2]);
          return;
        }
        else if (ABREV_EQ("OBJECT", arg[0]))
        {
          clone_obj(arg[1], arg[2]);
          return;
        }
        else 
        {
          printf("I don't understand %s.\n", arg[0]);
          printf("clone <LOC|MOB|OBJ> <existing> <new name>\n");
          return;
        }
          
      case -1:
        if (!quiet)
          printf("Huh??? What ya mean?\n");
        else
          printf("Don't understand the call from game_com()\n");
        return;

      default:
        not_imp();
        break;
    }
  }
  if (!quiet)
    printf("Ok\n");
  return;
}
  
void not_imp()
{
  printf("Not yet implemented\n");
  return;
}

/*
**  check, goes through the mobiles, objects and locations reporting
**  errors that it is unable to repair itself. It will try to fix all
*/
void check()
{
  PLOC l = loc;
  PMOB m = mob;
  POBJ o = obj, o2;
  int  i;

  while (l != NULL)
  {
    fix_exit(l);
    for (i = 0; i < NEXITS; i++)
    {
      if ((l->exit_type[i] != X_NONE) && (l->exits[i] == NULL))
        printf("Location %s has a null exit to %s\n", l->name, l->ename[i]);
    }
    l = l->next;
  }
  while (m != NULL)
  {
    if (m->loc == NULL)
      m->loc = flbname(m->locname);
    if (m->loc == NULL)
      printf("Mobile %s is in no existant location %s.\n",m->name, m->locname);
    m = m->next;
  }
  while (o != NULL)
  {
    fix_obj(o);
    if (o->location == NULL)
      printf("Object %s is in non-existant location %s.\n", o->name, o->lname);
    o2 = o->next;
    while (o2 != NULL)
    {
      if (EQ(o2->name, o->name))
      {
        printf("Object %s: %d(%s) has a the same NAME as %d(%s)\n",
                o->name, o->number, o->pname, o2->number, o2->name);
      }
      o2 = o2->next;
    }
    if (!EMPTY(o->link))
    {
      o2 = fobname(o->link);
      if (o2 != NULL)
      {
        if (EMPTY(o2->link) || !EQ(o->name, o2->link))
          printf("Object %s is linked to %s, but link doesn't go back\n",
                  o->name, o2->name);
      }
      else
        printf("Object %s is linked to a non-existant object called %s\n",
               o->name, o->link);
    }    
    o = o->next;
  }
}
   
/*
**  globals either displays or sets a global variable. They are things that
**  tells the editor if the mud you have knows about features some muds have
**  and others don't.
*/
void globals()
{
  Bool flg;
  int i,a;
  
  if (EMPTY(arg[0]))
  {
    
    printf("\t\t The global/configuration values are :\n\n");
    printf("Brief    = %s     Counters  = %s     Color     = %s\n",
           btos(brief), btos(o_counters), btos(color));
    printf("Verbose  = %s", btos(verbose));

#ifdef _NO_WFLAGS
    printf("     Wflags    = (compiled out)\n");
#else
    printf("     Wflags    = %s\n", btos(wear_flags));
#endif

    printf("\n\t\t\tFlag name #total/#used\n");
    for (i = 0, a = 0; o_flags[i] != TABLE_END; i++)
      if (!EMPTY(o_flags[i]))
        a++;
    printf("Oflags %3d/%-3d      ",NUM_OFLAGS, a);

    for (i = 0, a = 0; l_flags[i] != TABLE_END; i++)
      if (!EMPTY(l_flags[i]))
        a++;
    printf("Lflags %3d/%-3d      ", NUM_LFLAGS, a);

    for (i = 0, a = 0; p_flags[i] != TABLE_END; i++)
      if (!EMPTY(p_flags[i]))
        a++;
    printf("Pflags %3d/%-3d\n", NUM_PFLAGS, a);

    for (i = 0, a = 0; s_flags[i] != TABLE_END; i++)
      if (!EMPTY(s_flags[i]))
        a++;
    printf("Sflags %3d/%-3d      ", NUM_SFLAGS, a);

    for (i = 0, a = 0; m_flags[i] != TABLE_END; i++)
      if (!EMPTY(m_flags[i]))
        a++;
    printf("Mflags %3d/%-3d      ", NUM_MFLAGS, a);

#ifdef _NO_WFLAGS
    printf("Wflags (compiled out)\n");
#else
    for (i = 0, a = 0; w_flags[i] != TABLE_END; i++)
      if (!EMPTY(w_flags[i]))
        a++;
    printf("Wflags %3d/%-3d\n", NUM_WFLAGS, a);
#endif

    printf("\n# of Objects = %-6d  # of Mobiles = %-6d  # of Locations %-6d\n", 
           numobj, nummob, numloc);
    if (modified)
      printf("The zone has been MODIFIED");
    printf("\nmyloc    = %s[%s]\n", (myloc == NULL) ? "<null>" : myloc->pname,
           (myloc == NULL) ? "<null>" : myloc->name);
    return;
  }
  if (EQ("SAVE", arg[0]))
  {
    save_glob();
    printf("Saved\n");
    return;
  }

  if (EMPTY(arg[1]))
  {
    printf("Set it to what?\n");
    return;
  }
  if (ABREV_EQ("FALSE", arg[1]))
    flg = FALSE;
  else if (ABREV_EQ("TRUE", arg[1]))
    flg = TRUE;
  else
  {
    flg = FALSE;
  }
  switch(lookup(glob_table,arg[0]))
  {
    case 0:
      brief = flg;
      break;
    case 1:
      o_counters = flg;
      break;
    case 2: /* Oflags */
    case 3:
    case 4:
    case 5:
    case 6:
      printf("Use the define/undefine commands to add/remove flags\n");
      break;
    case 7:
      verbose = flg;
      break;
    case 8:
    case 9: /* Wflags */
#ifdef _NO_WFLAGS
      no_wflags();
#else
      wear_flags = flg;
#endif
    default:
      printf("You can't set %s\n",arg[0]);
  }
  return;
}

/*
**  Places a mobile in the same room as you, an object in your hands
*/
void summon(s)
	char *s;
{
  POBJ o = NULL;
  PMOB m = NULL;

  if (EMPTY(s))
  {
    printf("Summon what?\n");
    return;
  }
  
  m = fmobile(s);
  o = fobject(s);

  if ((m == NULL) && (o == NULL))
  {
    if (EMPTY(arg[1]))
    {
      printf("I can't find that.\n");
      return;
    }
    if (ABREV_EQ("MOBILE", s))
      m = fmobile(arg[1]);
    else if (ABREV_EQ("OBJECT", s))
      o = fobject(arg[1]);
  }
  if (m != NULL)
  {
    m->loc = myloc;
   printf("You utter the some magic words and %s appears in a puff of smoke\n",
          m->pname);
  }
  else if (o != NULL)
  {
    o->ltype = UNKNOWN;
    FREE(o->lname);
    o->location = NULL;
   printf("There is a small flash of light and the %s appears in your hands\n",
          o->pname);
  }
  else
    printf("I Can't seem to find that.\n");
  return;
}
     
/*
**  Show the examine text for a mobile or object.
*/
void examine(s)
	char *s;
{
  PMOB m = NULL;
  POBJ o = NULL;

  if (EMPTY(s))
  {
    printf("Examine what?\n");
    return;
  }
  o = fobject(s);
  if (o != NULL)
  {
    printf("You examine the %s\n",o->pname);
    if (EMPTY(o->examine))
      printf("You see nothing special about it.\n");
    else
      write_string(o->examine, stdout);
    return;
  }
  m = fmobile(s);
  if (m != NULL)
  {
    printf("You examine them\n");
    if (EMPTY(m->exam))
      printf("You see nothing special about them.\n");
    else
      write_string(m->exam, stdout);
    return;
  }
  printf("Examine who or what?\n");
  return;
}

/*
**  Allows you to define a new S/P/L/Mflag as you edit the zone
*/
void define(flg,tab)
	char *flg;
	char *tab;
{
  if (EMPTY(flg) || EMPTY(tab))
  {
    printf("define Xflag flag\n");
    return;
  }
  
  switch (lookup(glob_table, tab))
  {
    case 2:
      if (lookup(o_flags,flg) != -1)
      {
        printf("Thats Oflag already exists\n");
        return;
      }
      create_flag(flg, o_flags); 
      break;
    case 3:
      if (lookup(l_flags,flg) != -1)
      {
        printf("Thats Lflag already exists\n");
        return;
      }
      create_flag(flg, l_flags); 
      break;
    case 4:
      if (lookup(p_flags,flg) != -1)
      {
        printf("Thats Pflag already exists\n");
        return;
      }
      create_flag(flg, p_flags); 
      break;
    case 5:
      if (lookup(m_flags,flg) != -1)
      {
        printf("Thats Mflag already exists\n");
        return;
      }
      create_flag(flg, m_flags); 
      break;
    case 6:
      if (lookup(s_flags,flg) != -1)
      {
        printf("Thats Sflag already exists\n");
        return;
      }
      create_flag(flg, s_flags); 
      break;
    case 9:
#ifdef _NO_WFLAGS
      no_wflags();
#else
      if (lookup(w_flags,flg) != -1)
      {
        printf("Thats Wflag already exists\n");
        return;
      }
      create_flag(flg, w_flags); 
#endif
      break;
    default:
      printf("Define what\n");
  }
  return;
}

/*
**  Allows you to undef a certain L/P/S/Mflag while editing zone
*/
void undefine(flg,tab)
	char *flg;
	char *tab;
{
  int i;

  switch (lookup(glob_table, tab))
  {
    case 2:
      if ((i = lookup(o_flags,flg)) != -1)
      {
        FREE(o_flags[i]);
      }
      else
        printf("Can't find that Oflag\n");
      break;
    case 3:
      if ((i = lookup(l_flags,flg)) != -1)
      {
        FREE(l_flags[i]);
      }
      else
        printf("Can't find that Lflag\n");
      break;
    case 4:
      if ((i = lookup(p_flags,flg)) != -1)
      {
        FREE(p_flags[i]);
      }
      else
        printf("Can't find that Pflag\n");
      break;
    case 5:
      if ((i = lookup(m_flags,flg)) != -1)
      {
        FREE(m_flags[i]);
      }
      else
        printf("Can't find that Mflag\n");
      break;
    case 6:
      if ((i = lookup(s_flags,flg)) != -1)
      {
        FREE(s_flags[i]);
      }
      else
        printf("Can't find that Sflag\n");
      break;
    case 9:
#ifdef _NO_WFLAGS
      no_wflags();
#else
      if ((i = lookup(w_flags,flg)) != -1)
      {
        FREE(w_flags[i]);
      }
      else
        printf("Can't find that Wflag\n");
#endif
      break;
    default:
      printf("Undefine what\n");
  }
  return;
}

/*
**  Just forces the user to press return
*/
void my_pause()
{
  printf("\n****** Press RETURN to continue ******");
  NEWLINE(stdin);
  return;
}

void usage(x)
	int x;
{
  printf("\n ABERED:- AberMUD(dirt3) zone editor version %s %s\n",
         VERSION, "Copyright (C) J.Willie");
  printf("--------------------------------------------------------%s\n",
         "----------------");

  if (!x)
    return;

  printf("\nSee README file for additional info about copying/usage\n\n");
  printf("Credits:       -Author   : James Willie (Ithor)\n");
  printf("               -Debugging: Mary Jones (Larken)\n\n");
  
  if (x < 2)
    return;

  printf("Usage: abered zonename\n");
  printf("       eg abered eforest (to edit elven forest)\n");
  exit(0);
}

#ifdef _NO_WFLAGS
void no_wflags()
{
  printf("Wflags have been deliberately compiled out of this version for\n");
  printf("lack of memory.\n");
  return;
}
#endif

#ifdef IBM_MLJ_EDIT
void comments()
{
  char *p;

  p = edit_string(comment, 1);
  if ((p == NULL) && (abort_edit()))
  {
    printf("Ok, comments not changed.\n");
    return;
  }
  if (comment != NULL)
    free(comment);
  comment = p;
  return;
}  
#else
void comments()
{
  printf("This version does not support comment editing");
  printf("\nThe current comment is:\n%s\n", 
         ((comment == NULL) ? "NULL" : comment));
  return;
}
#endif
