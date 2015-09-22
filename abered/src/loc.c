/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    Function related to loctaions including those required to change them  *
 *  and those required to move around the editor.                            *
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
static void rename_all(PLOC l, char *n);
static Bool check_dup_name(char *s);
static void free_loc(PLOC l);
#else
static void rename_all(l,n);
static Bool check_dup_name(s);
static void free_loc(l);
#endif

static char *dir[] = { 
	"North:",	"East:",	"South:",
	"West:",	"Up:",		"Down:"
};

/*
**  Find location by name
*/
PLOC flbname(s)
	char *s;
{
  PLOC l = loc;
  
  if (EMPTY(s))
    return NULL;

  while (l != NULL)
  {
    if (strcasecmp(l->name, s) == 0)
      return l;
    l = l->next;
  }
  return NULL;
}

/*
**  Connect exits of locations to their rooms or objects. 
*/
void fix_exit(l)
	PLOC l;
{
  int i;

  if (l == NULL)
    return;
 
  for (i = 0; i < NEXITS; i++)
  {
    switch(l->exit_type[i])
    {
      case X_NORMAL:
        l->exits[i] = (PLOC)flbname(l->ename[i]);
        break;
      case X_OBJECT:
        l->exits[i] = (POBJ)fobname(l->ename[i]);
        break;
      default:
        l->exits[i] = NULL;
        break;
    }
  }
  return;
}

/*
**  Fix all the exits
*/
void fix_exits()
{
  PLOC l = loc;

  while (l != NULL)
  {
    fix_exit(l);
    l = l->next;
  }
  return;
}

/*
**  look in a room see what the hecks there
*/
void lookin(l)
	PLOC l;
{
  POBJ o = obj;
  PMOB m = mob;

  if (l == NULL)
  {
    printf("\n A void beyond the universe as we know it\n");
    return;
  }
  printf("%s\n", l->pname);
  if (!brief)
    write_string(l->desc,stdout); /* This one likely to be to long */

  /* Display objects in the room */
  while (o != NULL)
  {
    if ((o->ltype == IN_ROOM) && ((PLOC)o->location == l) 
        && (!EMPTY(o->desc[o->state])))
    {
      printf("%s\n", o->desc[o->state]);
    }
    o = o->next;
  }

  /* Display mobiles in the room */
  while (m != NULL)
  {
    if (m->loc == l)
    {
      if (!EMPTY(m->desc))
        printf("%s\n",m->desc);
      else
        printf("%s is here.\n",m->pname);
    }
    m = m->next;
  }

  return;
}

/*
**  Lets move
*/
void dodirn(x)
	int x;
{
  PLOC l;
  POBJ o;

  if ((myloc == NULL) || (x >= NEXITS) || (x < 0))
  {
    printf("\033[1;35m*GIGGLE* *LAUGH* *ROFL*\033[0;0m\nOh, your serious :(\n");
    return;
  }

  if (myloc->exits[x] == NULL)
    fix_exit(myloc);

  switch(myloc->exit_type[x])
  {
    case X_NORMAL:
      l = (PLOC)myloc->exits[x];
      break;

    case X_OBJECT:
      l = NULL;
      if (myloc->exits[x] != NULL)
      {
        o = fobname(((POBJ)myloc->exits[x])->name);
        if (o != NULL)
        {
          o = fobname(o->link);
          if ((o != NULL) && (o->ltype == IN_ROOM))
            l = (PLOC)o->location;
          else
            printf("Link object unknown\n");
        }
        else
          printf("Pointer to unknown object\n");
      }
      else
        printf("NULL exit\n");
      break;
    
    case X_NONE:
      printf("No exit that way\n");
      return;
      break;

    default:
      printf("Unknown exit type\n");
      l = NULL;
      break;
  } 

  if (l == NULL)
    printf("Error somewhere :(\n");
  else
  {
    myloc = l;
    lookin(myloc);
  }
  return;
}   
  
/*
**  Show stats of a room, in dirt loc format, default room is myloc
*/
void rstatscom()
{
  PLOC l;

  if (EMPTY(arg[0]))
    l = myloc;
  else if ((l = flbname(arg[0])) == NULL)
    l = myloc;

  write_dirt_loc(l,stdout);
  return;
}

/*
**  Show list if exits in the room
*/
void exitcom()
{
  int i;

  if (myloc == NULL)
  {
    printf("None\n");
    return;
  }

  if (!EMPTY(arg[0]))
  {
    i = lookup(dir,arg[0]);
    if ((i < 0) || (i >= NEXITS))
    {
      printf("What exit might that be?\n");
      return;
    }
    if (!EMPTY(arg[1]))
    {
      create_exit(myloc,i,arg[1]);
      printf("Exit created\n");
      return;
    }
    delete_exit(myloc,i);
    printf("Exit destroyed\n");
    return;
  }
    
  for(i = 0; i < NEXITS; i++)
  {
    if (myloc->exit_type[i] == X_NORMAL)
    {
      printf("%s%s%c\n", dir[i], myloc->ename[i], 
             (myloc->exits[i] == NULL) ? '*' : ' ');
    }
    else if (myloc->exit_type[i] == X_OBJECT)
    {
      printf("%s^%s%c\n", dir[i], myloc->ename[i], 
             (myloc->exits[i] == NULL) ? '*' : ' ');
    }
    else
    {
      printf("%sNone\n", dir[i]);
    }
  }
  return;
}
      
/*
**  goto the room in arg
**  Search for room, or object to match. (Objects must be in a room)
*/
void gotocom()
{
  PLOC l = NULL;
  POBJ o = NULL;
  PMOB m = NULL;

  if (arg[0] == NULL)
    l = NULL;
  else
    l = flbname(arg[0]);

  if (l == NULL)
  {
    m = fmobile(arg[0]);
    if (m != NULL) 
      l = (PLOC)m->loc;
  }

  if (l == NULL)
  {
    o = fobject(arg[0]);
    if ((o != NULL) && (o->ltype == IN_ROOM))
      l = (PLOC)o->location;
  }
  if (l == NULL)
    printf("Where's that?\n");
  else
  {
    myloc = l;
    lookin(myloc);
  }
}

void create_exit(l,x,s)
	PLOC l;
	int x;
	char *s;
{
  char *t;
  
  if ((x < 0) || (x > NEXITS))
    return;

  if (s[0] == '^')
  {
    t = &(s[1]);
    l->exit_type[x] = X_OBJECT;
    l->exits[x] = fobname(t);
  }
  else
  {
    t = s;
    l->exit_type[x] = X_NORMAL;
    l->exits[x] = (PLOC)flbname(t);
  }
  l->ename[x] = COPY(t);
  modified = TRUE;
}

/*
**  Delete one of the exits
*/
void delete_exit(l,x)
	PLOC l;
	int x;
{
  if ((x < 0) || (x >= NEXITS))
    return;

  l->exit_type[x] = X_NONE;
  FREE(l->ename[x]);
  l->exits[x] = NULL;
  modified = TRUE;
  return;
}

/*
**  This is infact no editor, merely allows you to enter text for the room 
**  description. If you want to edit, use an editor later on.
*/
Bool redit_com()
{
  char *s;
  PLOC l = myloc;
  int which = 0, i;

  if (arg[0] != NULL)
  {
    if (arg[1] != NULL)
    {
      if (ABREV_EQ("NAME", arg[0]))
        which = 2;
      else if (ABREV_EQ("PNAME", arg[0]))
        which = 1;
      else if (ABREV_EQ("DESCRIPTION", arg[0]))
        which = 0;
      else
      {
        printf("redit <DESCRIPTION|PNAME|NAME> <LOCATION>\n");
        printf("The default is desc, eg ledit loc == ledit desc loc\n");
        return FALSE;
      }
      arg[0] = arg[1];
      l = flbname(arg[0]);
    }
    else
    {
      if (ABREV_EQ("NAME", arg[0]))
        which = 2;
      else if (ABREV_EQ("PNAME", arg[0]))
        which = 1;
      else if (ABREV_EQ("DESCRIPTION", arg[0]))
        which = 0;
      else
        l = flbname(arg[0]);
    }
  }
  if (l == NULL)
  {
    printf("Don't know that room.\n");
    return FALSE;
  }
  switch(which)
  {
    case 0:
#ifdef IBM_MLJ_EDIT
      set_header("\tEnter your room description");
      s = edit_string(l->desc, 1);     
#else      
      printf("Enter the new description for room %s\nPress ^<CR> to end\n",
             l->pname);
      s = get_text(stdin,'^');
      NEWLINE(stdin);
#endif
      if ((s == NULL) || (s[0] == '\0'))
      {
        printf("No change\n");
        return FALSE;
      }
      free(l->desc);
      l->desc = s;
      break;

    case 1:
      printf("Enter the new pname for room %s\nHit <CR> when done\n",l->pname);
      s = get_text(stdin, '\n');
      if ((s == NULL) || (s[0] == '\0'))
      {
        printf("No change\n");
        return FALSE;
      }
      free(l->pname);
      l->pname = s;
      break;

    case 2:
      printf("Enter the new name for room %s\nONE word only\n", l->pname);
      s = get_text(stdin, '\n');
      if ((s == NULL) || (s[0] == '\0') || check_dup_name(s))
      {
        printf("No change\n");
        return FALSE;
      }
      for(i = 0; (!isspace(s[i])) && (s[i] != '\0'); i++)
        ;
      s[i] = '\0';
      rename_all(l,s);
      free(l->name);
      l->name = COPY(s);
      FREE(s);
      break;
  }
  modified = TRUE;
  return TRUE;
}

/*
**  Change the names on exits, objects etc, when a room name changes.
**  If n == NULL, it just deletes the exit, or sets mob/obj loc to NULL
*/
static void rename_all(l,n)  
	PLOC l;
	char *n;
{
  PLOC r = loc;
  int i;

  while(r != NULL)
  {
    for(i = 0; i < NEXITS; i++)
      if (r->exits[i] == l)
      {
        free(r->ename[i]);
        if (n == NULL)
        {
          r->ename[i] = NULL;
          r->exit_type[i] = X_NONE;
          r->exits[i] = NULL;
        }
        else
          r->ename[i] = COPY(n);
      }
    r = r->next;
  } 
  return;
}

/*
**  Set/show lflags. It can optionally take an argument of which room
**  but defaults to player/editors current location
*/
void lflags()
{
  int k;
  PLOC l;
  char *f, *t;

  if ((l = flbname(arg[0])) == NULL)  /* find location we a looking at    */
  {
    l = myloc;                        /* 2 args, def loc is myloc         */
    f = arg[0];
    t = arg[1];
  }
  else                                /* found loc, look arg 1,2 for args */
  {
    f = arg[1];
    t = arg[2];
  }

  if (l == NULL)
  {
    printf("What location?\n");
    return;
  }

  if (EMPTY(f))                       /* Query all flags in the room     */
  {
    for (k = 0; k < NUM_LFLAGS; k++)
    {
      if (l->lflags[k] == TRUE)
      {
        printf("%s ",l_flags[k]);
      }
    }
    printf("\n");
    return;
  }
  
  k = lookup(l_flags, f);
  if (k < 0)
  {
    printf("No such flag\n");
    return;
  }

  if (EMPTY(t))                       /* Query flag f                    */
  {
    printf("%s", arg[0]);
    if (l->lflags[k])
      printf(" is true.\n");
    else 
      printf(" is false.\n");
  }
  else if (ABREV_EQ("TRUE",t))        /* Set flag f, to t                */
  {
    l->lflags[k] = TRUE;
    modified = TRUE;
  }
  else if (ABREV_EQ("FALSE",t))
  {
    l->lflags[k] = FALSE;
    modified = TRUE;
  }
  else
    printf("Set it TRUE or FALSE?\n");
  return;
}

/*
**  print a list of locations
*/
void list_loc()
{
  PLOC l = loc;
  int i,j;

  while(l != NULL)
  {
    j = strlen(l->name);
    printf("%s",l->name);
    for(i = j; i <= 20; i++)
      putc(' ', stdout);
    printf("%s\n",l->pname);
    l = l->next;
  }
}

/*
**  Create a complete new location
*/
void create_loc()
{
  PLOC l, tmp;
  char *name = arg[0];
  int i;

  l = NEW(LOC);
  
  /* Initialise the data structure */
  l->name = COPY("NEW");
  l->pname = COPY("NEW");
  l->desc = COPY("NEW");
  for (i = 0; l_flags[i] != TABLE_END; i++)
    l->lflags[i] = FALSE;
  for(i = 0; i < NEXITS; i++)
  {
    l->exits[i] = NULL;
    l->ename[i] = NULL;
    l->exit_type[i] = X_NONE;
  }
  /* Move me to the new room and remember old one in case creation fails */
  
  tmp = myloc;
  myloc = l;

  /* Rooms name (MUST be unique) */
  if (EMPTY(name) || check_dup_name(name))
  {
    set_args("NAME",NULL,NULL);
    if (redit_com() == FALSE)
    {
      free_loc(l);
      myloc = tmp;
      return;
    }
  }
  else
    l->name = COPY(name);

  /* The rooms Pname */
  set_args("PNAME", NULL, NULL);
  if (redit_com() == FALSE)
  {
    free_loc(l);
    myloc = tmp;
    return;
  }
  
  /* New description for the room */
  free_args();
  redit_com();

  /* Insert new room somewhere in the list of rooms */
  if (loc == NULL)
  {
    myloc = l;  
    loc = l;
    loc->next = NULL;
  }
  else
  {
    l->next = tmp->next;
    tmp->next = l;
  }
  modified = TRUE;
  numloc++; 

  return;
}

/*
**  Destroy a location, if myloc is to be destroyed, move me, and fix the 
**  exits into the location.
*/
void do_destroy_loc(l)
	PLOC l;
{
  PLOC r = loc;
  PMOB m = mob;
  POBJ o = obj;

  if (l == NULL)
    return;

  rename_all(l, NULL); /* Delete all exits from other rooms into here */
  /* Delete mobile location pointers to the room */
  while (m != NULL)
  {
    if (m->loc == l)
      m->loc = NULL;
    m = m->next;
  }
  /* Delete object location pointers to the room */
  while (o != NULL)
  {
    if ((o->ltype == IN_ROOM) && (((PLOC)o->location) == l))
      o->location = NULL;
    o = o->next;
  }
    
  if (loc == l)
  {
    loc = l->next;
  }
  else
  {
    for(r = loc; r != NULL; r = r->next)
      if (r->next == l)
        break;
    r->next = l->next;
  }

  if (l == myloc)
  {
    printf("The universe begins to crackle, distant suns explode, as you \n");
    printf("enjoy your dinner at millyways\n");
    printf("When it's all over you find yourself, safe in a new location\n");
    myloc = loc;
  }
  free_loc(l);
  modified = TRUE;
  numloc--;
  return;
}

/*
**  The destroy location command.
*/
void destroy_loc()
{
  PLOC l;
  
  if (EMPTY(arg[0]))
  {
    printf("Destroy what room?\n");
    return;
  }
  l = flbname(arg[0]);
  if (l == NULL)
  {
    printf("The room is only in YOUR imagination, how am I supposed to ");
    printf("destroy it\n");
    return;
  }
  printf("Are you sure you want to destroy %s [%s] [NO/yes] ? ",
         l->name,l->pname);
  get_command();
  pnt = cmdln;
  cmd = get_arg();
  if (EMPTY(cmd) || !ABREV_EQ(cmd,"Yes"))
  {
    printf("Location _NOT_ destroyed\n");
    return;
  }
  do_destroy_loc(l);
  printf("Location destroyed\n");
  return;
}

static Bool check_dup_name(s)
	char *s;
{
  if (flbname(s) != NULL)
  {
    printf("\nSorry, a location exist with that name. [Try adding a number]\n");
    return TRUE;
  }
  return FALSE;
}

static void free_loc(l)
	PLOC l;
{

  if (l == NULL)
    return;

  FREE(l->name);
  FREE(l->pname);
  FREE(l->desc);
  FREE(l);
  return;
}

/*
**  clone_loc() exactly clones another room, lflags, description etc
*/
void clone_loc(s, t)
	char *s;
	char *t;
{
  PLOC l, tmp;
  int i;

  tmp = flbname(s);
  if (tmp == NULL)
  {
    /* Maybe they gave args in wrong order */
    tmp = flbname(t);
    if (tmp == NULL)
    {
      printf("Error: I can't find room %s to clone it. Sorry!\n", s);
      return;
    }
    t = s;
  }
  
  l = flbname(t);
  if (l != NULL)
  {
    printf("Error: Sorry can't clone %s to %s because destination exists.\n",
           tmp->name, t);
    return;
  }
  
  l = NEW(LOC);

  /* Initialise the data structure */
  l->pname = COPY((char *)tmp->pname);
  l->name = COPY(t);
  l->desc = COPY((char *)tmp->desc);
  for (i = 0; l_flags[i] != TABLE_END; i++)
    l->lflags[i] = tmp->lflags[i];
  for(i = 0; i < NEXITS; i++)
  {
    l->exits[i] = tmp->exits[i];
    if ((tmp->exits[i] == NULL) && (!EMPTY(tmp->ename[i])))
      l->ename[i] = COPY((char *)tmp->ename[i]);
    else
      l->ename[i] = NULL;
    l->exit_type[i] = tmp->exit_type[i];
  }

  /* Insert new room into list */
  if (loc == NULL)
  {
    myloc = l;  
    loc = l;
    loc->next = NULL;
  }
  else
  {
    l->next = tmp->next;
    tmp->next = l;
  }
  printf("Cloned location `%s' [%s] to [%s].\n", l->pname, tmp->name, l->name);
  modified = TRUE;
  numloc++;
  return;
}
