/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    The file contains functions that relate to objects only. The io for    *
 *  objects is handled in obj_io.c                                           *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

extern char *obj_table[];
extern char *olt_table[];

/*
**  returns a pointer to the name of the location the object is in.
*/
char *oloc_name(o)
	POBJ o;
{
  if (o->location == NULL)
    return (o->lname == NULL) ? "You must have it" : o->lname;

  switch((int)o->ltype)
  {
    case IN_ROOM:
      return ((PLOC)o->location)->name;
    case IN_CONTAINER:
      return ((POBJ)o->location)->name;
    case CARRIED_BY:
    case WORN_BY:
    case WIELDED_BY:
    case BOTH_BY:
      return ((PMOB)o->location)->name;
    default:
      break;
  }
  return "Eeeek!! I've lost it :(";
}

void list_obj()
{
  POBJ o = obj;

  printf("Number  Name            Pname           Location\n");
  while(o != NULL)
  {
    printf("[%4d]  %-16s%-16s%s:%s\n",o->number, o->name, o->pname,
           olt_table[(int)o->ltype], oloc_name(o));
    o = o->next;
  }
  return;
}

/*
**  FOB functions all find objects by (name, pname, altname or number)
**  fobject(s) looks for them in the above order.
*/
POBJ fobject(s)
	char *s;
{
  POBJ o;

  if (EMPTY(s))
    return NULL;

  o = fobname(s);
  if (o == NULL)
    o = fobpname(s);
  if (o == NULL)
    o = fobaname(s);
  if (o == NULL)
    o = fobnumber(s);
  return o;
}
  
POBJ fobname(s)
	char *s;
{
  POBJ o = obj;

  while(o != NULL)
  {
    if (EQ(o->name, s))
      return o;
   o = o->next;
  }
  return NULL;
}

/* 
**  Look for on ein my room first
*/
POBJ fobpname(s)
	char *s;
{
  POBJ o = obj;
  POBJ t = NULL;

  while(o != NULL)
  {
    if (EQ(o->pname, s))
    {
      if ((o->ltype == IN_ROOM) && (o->location == myloc))
        return o;
      else if (t == NULL)
        t = o;
    }
    o = o->next;
  }
  return t;
}

POBJ fobaname(s)
	char *s;
{
  POBJ o = obj;
  POBJ t = NULL;

  if (EMPTY(s))
    return NULL;

  while(o != NULL)
  {
    if (!EMPTY(o->altname) && EQ(o->altname, s))
    {
      if ((o->ltype == IN_ROOM) && (o->location == myloc))
        return o;
      else if (t == NULL)
        t = o;
    }
    o = o->next;
  }
  return t;
}

POBJ fobnumber(s)
	char *s;
{
  POBJ o = obj;
  int num = -1;

  num = atoi(s);
  while(o != NULL)
  {
    if (o->number == num)
      return o;
   o = o->next;
  }
  return NULL;
}

/*
**  Show the stats on an object
*/
void ostat(s)
	char *s;
{
  POBJ o = NULL;

  o = fobject(s);
  if (o == NULL)
  {
    printf("No such object\n");
    return;
  }
  write_dirt_obj(stdout,o);
  return;
}
  
/*
**  fixes up object locations
*/
void fix_obj(o)
	POBJ o;
{
  if (o == NULL)
    return;
 
  if (o->location != NULL)
    return;
    
  switch(o->ltype)
  {
    case IN_ROOM:
      o->location = flbname(o->lname);
      break;
    case IN_CONTAINER:
      o->location = fobname(o->lname);
      break;
    case CARRIED_BY:
    case WIELDED_BY:
    case WORN_BY:
    case BOTH_BY:
      o->location = fmobile(o->lname);
      break;
    default:
      o->location = NULL;
      break;
  }
  return;
}

void fix_objs()
{
  POBJ o = obj;

  while(o != NULL)
  {
    fix_obj(o);
    o = o->next;
  }
  return;
}

/*
**  Set/Show the oflags on an object.
*/
void oflags()
{
  int k;
  POBJ o = NULL;

  if (EMPTY(arg[0]))
  {
    printf("Oflags for what?\n");
    return;
  }
  
  if ((o = fobject(arg[0])) == NULL)
  {
    printf("No such object, sorry.\n");
    return;
  }

  if (EMPTY(arg[1]))                     /* Query the flags on object       */
  {
    for (k = 0; k < NUM_OFLAGS; k++)
    {
      if (o->oflags[k] == TRUE)
      {
        printf("%s ",o_flags[k]);
      }
    }
    printf("\n");
    return;
  }
  
  k = lookup(o_flags, arg[1]);
  if (k < 0)
  {
    printf("No such oflag\n");
    return;
  }

  if (EMPTY(arg[2]))                     /* Query particular flag           */
  {
    printf("%s", arg[0]);
    if (o->oflags[k])
      printf(" is true.\n");
    else 
      printf(" is false.\n");
  }
  else if (ABREV_EQ("TRUE",arg[2]))      /* Set the flag to arg[2]          */
  {
    o->oflags[k] = TRUE;
    modified = TRUE;
  }
  else if (ABREV_EQ("FALSE",arg[2]))
  {
    o->oflags[k] = FALSE;
    modified = TRUE;
  }
  else
    printf("Set it TRUE or FALSE?\n");
  return;
}

/*
**  Set/Show/Clear Wflags
*/
#ifdef _NO_WFLAGS

void wflags()
{
  no_wflags();
  return;
}

#else

void wflags()
{
  int k;
  POBJ o = NULL;

  if (wear_flags == FALSE)
  {
    printf("Warning, Wflags are not enabled and won't save.\n");
  }

  if (EMPTY(arg[0]))
  {
    printf("Wflags for what?\n");
    return;
  }
  
  if ((o = fobject(arg[0])) == NULL)
  {
    printf("No such object, sorry.\n");
    return;
  }

  if (EMPTY(arg[1]))                     /* Query the flags on object       */
  {
    for (k = 0; k < NUM_WFLAGS; k++)
    {
      if (o->wflags[k] == TRUE)
      {
        printf("%s ",w_flags[k]);
      }
    }
    printf("\n");
    return;
  }
  
  k = lookup(w_flags, arg[1]);
  if (k < 0)
  {
    printf("No such wflag\n");
    return;
  }

  if (EMPTY(arg[2]))                     /* Query particular flag           */
  {
    printf("%s", arg[0]);
    if (o->wflags[k])
      printf(" is true.\n");
    else 
      printf(" is false.\n");
  }
  else if (ABREV_EQ("TRUE",arg[2]))      /* Set the flag to arg[2]          */
  {
    o->wflags[k] = TRUE;
    modified = TRUE;
  }
  else if (ABREV_EQ("FALSE",arg[2]))
  {
    o->wflags[k] = FALSE;
    modified = TRUE;
  }
  else
    printf("Set it TRUE or FALSE?\n");
  return;
}

#endif  /* _NO_WFLAGS */

/*
**  The main function involved with editing objects
*/
void oedit()
{
  POBJ o, o2;
  int  v = 0, what, q = 0, i;
  char s[MAX_COM+1];
  char *t = NULL, *p = NULL;

  if (EMPTY(arg[0]) || EMPTY(arg[1]))
  {
    printf("oedit what?\n");
    return;
  }

  o = fobject(arg[0]);
  if (o == NULL)
  {
    printf("No object called %s.\n",arg[0]);
    return;
  }

  what = lookup(obj_table, arg[1]);

  if (verbose)
    write_dirt_obj(stdout,o);
    
  if (EMPTY(arg[2]))
  {
    switch(what)
    {
      case O_NAME:
      case O_PNAME:
      case O_ANAME:
      case O_LINK:
        printf("Enter ONE word:");
        get_word(s,stdin);
        NEWLINE(stdin);
        t = COPY(s);
        break;

#ifdef IBM_MLJ_EDIT
      case O_EXAM:
        set_header("Object examine text editor.");
        t = edit_string(o->examine, 1);
        if ((t == NULL) && (abort_edit()))
          t = COPY(o->examine);
        break;
#else
      case O_EXAM:
        printf("\nEnter the text and finish with ^<CR>\n");
        t = get_text(stdin,'^');
        NEWLINE(stdin);
        break;
#endif

      case O_LOC:
        break;

      case O_VALUE:  q = o->value;    p = obj_table[O_VALUE];  break;
      case O_STATE:  q = o->state;    p = obj_table[O_STATE];  break;
      case O_COUNT:  q = o->counter;  p = obj_table[O_COUNT];  break;
      case O_DAMAGE: q = o->damage;   p = obj_table[O_DAMAGE]; break;
      case O_ARMOR:  q = o->armor;    p = obj_table[O_ARMOR];  break;
      case O_SIZE:   q = o->size;     p = obj_table[O_SIZE];   break;
      case O_WEIGHT: q = o->weight;   p = obj_table[O_WEIGHT]; break;
      case O_MAXS:   q = o->maxstate; p = obj_table[O_MAXS];   break;
#ifndef _NO_WFLAGS
      case O_WLEVEL: q = o->wlevel;   p = obj_table[O_WLEVEL]; break;
#endif
      default: /* Must be one that needs a number */
        break;
    }
    if (p != NULL)
    {
      printf("Enter new %s->%s[%d]:", o->name, p, q);
      v = get_int(stdin, &i);
      printf("\n");
      NEWLINE(stdin);
      if (i > 0)
      {
        printf("Value unchanged\n");
        return;
      }
    }
  }
  else
  {
    t = COPY(arg[2]);
    v = atoi(arg[2]);
  }
  fix_string(t);

  switch(what)
  {
    case O_NAME:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(o->name);
      o->name = COPY(t);
      break;

    case O_PNAME:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(o->pname);
      o->pname = COPY(t);
      break;

    case O_ANAME:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(o->altname);
      o->altname = COPY(t);
      break;

    case O_LINK:
      if (!EMPTY(o->link))
      {
        o2 = fobname(o->link);
        if (o2 != NULL)
          FREE(o2->link);
      }
      free(o->link);
      o->link = COPY(t);
      if ((o2 = fobname(t)) != NULL)
      {
        if (!EMPTY(o2->link))
        {
          printf("Warning: linking %s to %s, already linked to %s\n",
                 o->name, o2->name, o2->link);
          free(o2->link);
        }
        else
          printf("Linked %s and %s\n",o->name, o2->name);
        o2->link = COPY(o->name);
      }
      else
        printf("%s linked to non-existant object %s\n",o->name, t);
      break;

    case O_DESC:
      FREE(t);
      if ((v < 0) || (v >= MAX_STATES))
      {
        printf("You can't set desc[%d].\n",v);
        return;
      }
      if (!EMPTY(o->desc[v]))
        printf("[%s]\n",o->desc[v]);
      printf("Enter text for desc[%d]. (<CR> when done)\n", v);
      t = get_text(stdin, '\n');
      free(o->desc[v]); 
      o->desc[v] = COPY(t);
      break;
      
    case O_VALUE:
      o->value = v;
      printf("Base value changed to %d\n",o->value);
      break;

    case O_STATE:
      if ((v < 0) || (v >= MAX_STATES))
      {
        printf("Outside range for state[0 - 3]\n");
        FREE(t);
        return;
      }
      o->state = v;
      printf("Starting state changed\n");
      break;

    case O_LOC:
      if (EMPTY(t))
      {
        printf("\nEnter the location type:");
        get_word(s,stdin);
        NEWLINE(stdin);
        t = COPY(s);
      }
      v = lookup(olt_table, t);
      FREE(t);
      if (v <= UNKNOWN)
      {
        printf("Unknown location type.\n No change.\n");
        return;
      }
      o->ltype = v;
      t = get_arg();
      if (EMPTY(t))
      {
        printf("\nEnter the location name:");
        get_word(s,stdin);
        NEWLINE(stdin);
        t = COPY(s);
      }
      free(o->lname);
      o->lname = COPY(t);
      fix_obj(o);
      if (o->location == NULL)
        printf("Warning: Unknown location\n");
      break;

    case O_COUNT:
      o->counter = v;
      if (!o_counters)
        printf("WARNING: The value counter is not defined. See globals\n");
      printf("Counter set to %d\n", o->counter);
      break;

    case O_DAMAGE:
      o->damage = v;
      printf("Damage set to %d\n", o->damage);
      break;

    case O_WLEVEL:
#ifndef _NO_WFLAGS
      if (wear_flags == FALSE)
        printf("Warning, Wflags are not turned on and will not save.\n");
      o->wlevel = v;
      if (o->wlevel < 0)
      {
        printf("Turning off Wlevel.\n");
        o->wlevel = -1;
      }
      else
        printf("Wlevel set to %d\n", o->wlevel);
#else
      no_wflags();
#endif
      break;

    case O_ARMOR:
      o->armor = v;
      printf("Armor set to %d\n", o->armor);
      break;

    case O_WEIGHT:
      o->weight = v;
      printf("Weight set to %d\n", o->weight);
      break;

    case O_SIZE:
      o->size = v;
      printf("Size set to %d\n", o->size);
      break;

    case O_MAXS:
      if ((v < 0) || (v > MAX_STATES))
      {
        printf("MaxState unchanged, out of range value %d. Range [0 - %d]",
               v, MAX_STATES);
        FREE(t);
        return;
      }
      o->maxstate = v;
      printf("MaxState set to %d\n", o->maxstate);
      break;

    case O_EXAM:
      free(o->examine);
      o->examine = NULL;
      if (EMPTY(t))
        printf("Examine text deleted\n");
      else
      {
        o->examine = COPY(t);
        printf("Examine text changed\n");
      }
      break;

    default:
      printf("Sorry I do not know how to change a %s.\n",arg[1]);
      return;
  }
  modified = TRUE;
  FREE(t);
  return;
}

void link_door(s,t)
	char *s;
	char *t;
{
  char buf[MAX_COM + 1];

  if (EMPTY(s) || EMPTY(t))
  {
    printf("Must link something to something\n");
    return;
  }
  sprintf(buf, "oedit %s LINK %s", s, t);
  game_com(buf);
  return;
}
  
/*
**  Clone object s, and call it t
*/
void clone_obj(s, t)
	char *s;
	char *t;
{
  POBJ o, p;
  int j;

  p = fobject(s);
  if (p == NULL)
  {
    p = fobject(t);
    if (p == NULL)
    {
      printf("Object %s does not exist, sorry can't clone.\n", s);
      return;
    }
    t = s;
  }

  o = fobject(t);
  if (o != NULL)
  {
    printf("The object %s already exists, can't clone.\n", t);
    return;
  }

  o = NEW(OBJ);
  o->name = COPY(t);

  o->number = objnum;
  o->pname = COPY(p->pname);
  if (!EMPTY(p->link))
    printf("Warning, object %s is linked to %s. Link NOT copied.\n",
           p->name, p->link);
  o->link = NULL;
  if (EMPTY(p->altname))
    o->altname = NULL;
  else
    o->altname = COPY(p->altname);
  o->location = p->location;
  if ((p->location == NULL) && (!EMPTY(p->lname)))
    o->lname = COPY(p->lname);
  else
    o->lname = NULL;
  o->ltype = p->ltype;
  o->damage = p->damage;
  o->armor = p->armor;
  for(j = 0; j < NUM_OFLAGS; j++)
    o->oflags[j] = p->oflags[j];

#ifndef _NO_WFLAGS
  for(j = 0; j < NUM_WFLAGS; j++)
    o->wflags[j] = p->wflags[j];
  o->wlevel = p->wlevel;
#endif

  o->state = p->state;
  o->maxstate = p->maxstate;
  o->value = p->value;
  o->size = p->size;
  o->weight = p->weight;
  o->counter = p->counter;
  for(j = 0; j < MAX_STATES; j++)
  {
    if (EMPTY(p->desc[j]))
      o->desc[j] = NULL;
    else
      o->desc[j] = COPY(p->desc[j]);
  }
  if (EMPTY(p->examine))
    o->examine = NULL;
  else
    o->examine = COPY(p->examine);

  o->next = p->next;
  p->next = o;

  printf("Cloned %s[%s:%d] --> [%s:%d].\n", p->pname, p->name, p->number,
         o->name, o->number);
  numobj++;
  objnum++;
  modified = TRUE;
  return;
}

/*
**  Creates a new object, and places it in the object list.
*/
void ocreate()
{
  POBJ o;

  if (EMPTY(arg[0]))
  {
    printf("Please specify a UNIQUE name\n");
    return;
  }
  o = NEW(OBJ);
  init_obj(arg[0], o);
  numobj++;
  objnum++;
  o->next = obj;
  obj = o;
  modified = TRUE;
  return; 
}

/*
**  List the objects the editor is carrying, eg, the ones with ltype == UNKNOWN
*/
void inventory()
{
  POBJ o = obj;

  while (o != NULL)
  {
    if (o->ltype == UNKNOWN)
      printf("[%-12s:%-4d] %s(%s)\n", o->name, o->number, o->pname,
             (o->altname == NULL) ? "none" : o->altname);
    o = o->next;
  }
  return;
}

/*
**  Destroy the object s
*/
void ozap(s)
	char *s;
{
  POBJ o, o2 = obj;
  int i;

  o = fobject(s);
  if (o == NULL)
  {
    printf("Zap what object??\n");
    return;
  }
  printf("An enormous bolt of lightning arcs down from the sky turning the \n");
  printf("%s into a tiny pile of ash.\n", o->pname);
  while ((o2 != NULL) && (o2 ->next != o))
    o2 = o2->next;

  if (o2 == NULL)
    obj = o->next;
  else
    o2->next = o->next;

  for (o2 = obj; o2 != NULL; o2 = o2->next)
  {
    if ((o2->ltype == IN_CONTAINER) && (o == (POBJ)o2->location))
    {
      switch(o->ltype)
      {
        case CARRIED_BY:
        case WORN_BY:
        case WIELDED_BY:
        case BOTH_BY:
          o2->ltype = CARRIED_BY;
          break;
        default:
          o2->ltype = o->ltype;
      }
      o2->location = o->location;
    }
  }
  free(o->name);
  free(o->pname);
  free(o->altname);
  free(o->examine);
  free(o->lname);
  free(o->link);
  for (i = 0; i < MAX_STATES; i++)
    free(o->desc[i]);
  free(o);
  modified = TRUE;
  numobj--;
  return;
}

/*
**  Drop an object.
*/
void drop(s)
	char *s;
{
  POBJ o = NULL;

  if (EMPTY(s))
  {
    printf("Drop what?\n");
    return;
  }
  if (EQ("ALL", s))
  {
    for (o = obj; o != NULL; o = o->next)
    {
      if (o->ltype != UNKNOWN)
        continue;
      printf("[%s:%d] %s.\n", o->name, o->number, o->pname);
      if (myloc != NULL)
      {
        o->location = myloc;
        o->ltype = IN_ROOM;
      }
    }
    return;
  }   
  o = fobject(s);
  if ((o == NULL) || (o->ltype != UNKNOWN))
  {
    printf("You don't have it\n");
    return;
  }
  if (myloc == NULL)
    return;

  o->location = myloc;
  o->ltype = IN_ROOM;
  return;
}

void get(s)
	char *s;
{
  POBJ o;

  if (EMPTY(s) || ((o = fobject(s)) == NULL))
  {
    printf("Get what?\n");
    return;
  }
  o->location = NULL;
  o->ltype = UNKNOWN;
  modified = TRUE;
  return;
}

void give(s,t)
	char *s;
	char *t;
{
  POBJ o;
  PMOB m;

  if (EMPTY(s) || ((o = fobject(s)) == NULL))
  {
    printf("Give what to who?\n");
    return;
  }
  if (EMPTY(t) || ((m = fmobile(t)) == NULL))
  {
    printf("Give it to who?\n");
    return;
  }
  o->location = m;
  free(o->lname);
  o->lname = COPY(m->name);
  o->ltype = CARRIED_BY;
  modified = TRUE;
  return;
}

void put(s,t,z)
	char *s;
	char *t;
	char *z;
{
  POBJ o,p;

  if (EMPTY(s) || EMPTY(t) || ((o = fobject(s)) == NULL))
  {
    printf("Put what where?\n");
    return;
  }

  if (ABREV_EQ("DOWN", t))
  {
    if (myloc == NULL)
    {
      printf("You aren't in the world to put it down\n");
      return;
    }
    o->location = myloc;
    o->ltype = IN_ROOM;
    printf("You place the %s on the ground.\n", o->pname);
    return;
  }

  if (EQ("IN", t))
    t = z;

  if (EMPTY(t) || ((p = fobject(t)) == NULL))
  {
    printf("Put it in what?\n");
    return;
  }

  o->location = p;
  o->ltype = IN_CONTAINER;
  free(o->lname);
  o->lname = COPY(p->name);
  modified = TRUE;
  return;
}
