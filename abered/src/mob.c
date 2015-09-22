/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    The file contains all the commands directly to do with mobiles,        *
 *  displaying their statistics, listing them, editing them but not putting  *
 *  them in their rooms, since lookin() is a loc.c function.                 *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "prototyp.h"

extern char *Mob_tab[];

/*
**  Lists the mobiles in the editor
*/
void list_mob()
{
  PMOB m = mob;
  PLOC l;
  int i = 0;

  printf("Name            Title(Pname)            Location\n");
  while (m != NULL)
  {
    l = (PLOC)m->loc;
    printf("%-16s%-24s",m->name, m->pname);
    if (l == NULL)
      printf("%s*\n",m->locname);
    else
      printf("%s\n",l->name);
    m = m->next;
    i++;
  }
  printf("\nTotal = %d\n", i);
  nummob = i;
  return;
}

/*
**  Find mobiles by name, and by title
**  Unlike aber mud, it does NOT match the last word of the pname(title)
*/
PMOB fmobile(s)
	char *s;
{
  PMOB m = mob, t = NULL;

  while (m != NULL)
  {
    if (EQ(m->name, s))
      return m;
    if (EQ(m->pname, s))
    {
      if (myloc == m->loc)
        return m;
      else if (t == NULL)
        t = m;
    }
    m = m->next;
  }
  return t;
}
        
/*
**  mstat will show a mobiles stats if it exists.
*/
void mstat(s)
	char *s;
{
  PMOB m;

  m = fmobile(s);
  if (m == NULL)
  {
    printf("Don't know that mobile\n");
    return;
  }
  write_dirt_mob(m,stdout);
  return;
}

/*
**  Fix location of mobiles after all things have been read in.
*/
void fix_mobs()
{
  PMOB m = mob;

  while (m != NULL)
  {
    if (m->locname == NULL)
    {
      printf("Error: mobile %s does not have a location?\n", m->pname);
    }
    else
    {
      m->loc = flbname(m->locname);
    }
    m = m->next;
  }
  return;
}

/*
**  Set/Show the mflags, pflags or slfags on a mobile
*/
void mob_flags(n)
	int  n;
{
  int k, Num;
  PMOB m = NULL;
  char **tab;
  Bool *flg;

  if (EMPTY(arg[0]))
  {
    printf("flags for what mobile?\n");
    return;
  }
  
  if ((m = fmobile(arg[0])) == NULL)
  {
    printf("No such mobile, sorry.\n");
    return;
  }

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
      printf("Unknown flags\n");
      return;
      break;
  }

  if (EMPTY(arg[1]))   
  {
    for (k = 0; k < Num; k++)
    {
      if (flg[k] == TRUE)
      {
        printf("%s ",tab[k]);
      }
    }
    printf("\n");
    return;
  }
  k = lookup(tab, arg[1]);
  if (k < 0)
  {
    printf("No such flag\n");
    return;
  }

  if (EMPTY(arg[2]))
  {
    printf("%s:Flag %s",arg[0], tab[k]);
    if (flg[k] == TRUE)
      printf(" is true.\n");
    else 
      printf(" is false.\n");
  }
  else if (ABREV_EQ("TRUE",arg[2]))
  {
    flg[k] = TRUE;
    modified = TRUE;
  }
  else if (ABREV_EQ("FALSE",arg[2]))
  {
    flg[k] = FALSE;
    modified = TRUE;
  }
  else
    printf("Set it TRUE or FALSE?\n");
  return;
}

/*
**  The main function involved with editing mobiles
*/
void mset()
{
  PMOB m;
  int  v = 0, i, status, q = 0;
  char s[MAX_COM+1];
  char *t = NULL, *p = NULL;

  if (EMPTY(arg[0]) || EMPTY(arg[1]))
  {
    printf("medit what?\n");
    return;
  }

  m = fmobile(arg[0]);
  if (m == NULL)
  {
    printf("No mobile called %s.\n",arg[0]);
    return;
  }
  if (verbose)
    write_dirt_mob(m,stdout);
    
  i = lookup(Mob_tab, arg[1]);

  if (EMPTY(arg[2]))
  {
    switch(i)
    {
      case MOB_NAME:
        printf("Enter the new name(One word):");
        get_word(s,stdin);
        NEWLINE(stdin);
        t = COPY(s);
        break;

      case MOB_LOC:
        printf("Enter the new location(One word):");
        get_word(s,stdin);
        NEWLINE(stdin);
        t = COPY(s);
        break;

      case MOB_PNAME:
        printf("Enter the new title on one line.\n");
        t = get_text(stdin, '\n');
        break;

      case MOB_DESC:
        printf("Enter the new description. Hit return when done.\n");
        t = get_text(stdin, '\n');
        break;

#ifdef IBM_MLJ_EDIT
      case MOB_EXAM:
        set_header("Mobile examine text editor.");
        t = edit_string(m->exam, 1);
        break;
#else
      case MOB_EXAM:
        printf("\nEnter the text and finish with ^<CR>\n");
        t = get_text(stdin,'^');
        NEWLINE(stdin);
        break;
#endif
      
      case MOB_DAM:   q = m->damage;     p = Mob_tab[i]; break;
      case MOB_ARMOR: q = m->armor;      p = Mob_tab[i]; break;
      case MOB_AGG:   q = m->agg;        p = Mob_tab[i]; break;
      case MOB_SPEED: q = m->speed;      p = Mob_tab[i]; break;
      case MOB_STR:   q = m->hp;         p = Mob_tab[i]; break;

      default:
        break;
    }
    if (p != NULL)
    {
      printf("Enter new %s->%s[%d]:", m->name, p, q);
      v = get_int(stdin, &status);
      printf("\n");
      NEWLINE(stdin);
      if (status > 0)
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

  switch(i)
  {
    case MOB_NAME:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(m->name);
      m->name = t;
      break;

    case MOB_PNAME:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(m->pname);
      m->pname = t;
      break;

    case MOB_STR:
      m->hp = v;
      printf("Strength changed\n");
      break;

    case MOB_SPEED:
      m->speed = v;
      printf("Speed changed\n");
      break;

    case MOB_AGG:
      if ((v < 0) || (v > 100))
      {
        printf("Outside range for aggression[0 - 100]\n");
        return;
      }
      m->agg = v;
      printf("Aggression changed\n");
      break;

    case MOB_DAM:
      m->damage = v;
      printf("Damage changed\n");
      break;

    case MOB_ARMOR:
      m->armor = v;
      printf("Armor changed\n");
      break;

    case MOB_DESC:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      free(m->desc);
      m->desc = COPY(t); /* Just copy it again, incase we deleted the end */
      free(t);
      printf("Description changed\n");
      break;

    case MOB_EXAM:
      free(m->exam);
      if (EMPTY(t))
        printf("Examine text deleted\n");
      else
      {
        m->exam = COPY(t);
        printf("Examine text changed\n");
      }
      free(t);
      break;

    case MOB_LOC:
      if (EMPTY(t))
      {
        printf("No Change.\n");
        return;
      }
      m->loc = flbname(t);
      free(m->locname);
      m->locname = t;
      printf("Location changed\n");
      if (m->loc == NULL)
        printf("Warning, location %s does not exist\n", t);
      break;

    default:
      printf("Sorry I do not know how to change a %s.\n",arg[1]);
      return;
  }
  modified = TRUE;
  return;
}

/*
**  Create a new mobile and pop it into the list
*/
void mcreate(s)
	char *s;
{
  PMOB m = NULL;

  if (EMPTY(s))
  {
    printf("Create nothing, ok all done, I made nothing\n");
    return;
  }

  m = NEW(MOB);
  init_mob(m,s);
  if (myloc != NULL)
  {
    m->loc = myloc;
    m->locname = COPY(myloc->name);
  }
  m->next = mob;
  mob = m;
  printf("You magically create a new mobile.\n%s forms before your eyes.\n",
         m->pname);
  modified = TRUE;
  nummob++;
  return;
}
   
/*
**  clone a mobile s, and call it t
*/
void clone_mob(s, t)
	char *s;
	char *t;
{
  PMOB m, m1;
  int j;

  m1 = fmobile(s);

  if (m1 == NULL)
  {
    m1 = fmobile(t);
    if (m1 == NULL)
    {
      printf("Sorry I can't clone %s because it doesn't exist.\n", s);
      return;
    }
    t = s;
  }

  m = fmobile(t);
  if (m != NULL)
  {
    printf("Sorry, the destination must be a unique mobile name.\n");
    return;
  }

  m = NEW(MOB);

  /* Lets duplicate m1 and give it a new name */

  m->name = COPY(t);
  m->pname = COPY(m1->pname);
  m->level = m1->level;
  if (EMPTY(m1->locname))
    m->locname = NULL;
  else
    m->locname = COPY(m1->locname);
  m->loc = m1->loc;
  m->hp = m1->hp;
  m->armor = m1->armor;
  m->damage = m1->damage;
  m->agg = m1->agg;
  m->speed = m1->speed;
  for(j = 0; j < NUM_SFLAGS; j++)
    m->sflags[j] = m1->sflags[j];
  for(j = 0; j < NUM_MFLAGS; j++)
    m->mflags[j] = m1->mflags[j];
  for(j = 0; j < NUM_PFLAGS; j++)
    m->pflags[j] = m1->pflags[j];
  if (EMPTY(m1->desc))
    m->desc = NULL;
  else
    m->desc = COPY(m1->desc);
  if (EMPTY(m1->exam))
    m->exam = NULL;
  else 
    m->exam = COPY(m1->exam);

  /* Insert new mobile in list */
  m->next = m1->next;
  m1->next = m;

  printf("Time freezes for an instant and %s duplicates itself.\n", m->pname);
  printf("%s -> %s.\n", m1->name, m->name);
  modified = TRUE;
  nummob++;
  return;
}

void mzap(s,tg)
	char *s;
	Bool tg;
{
  PMOB m = mob, p = NULL;
  POBJ o = obj;

  if (EMPTY(s) || ((p = fmobile(s)) == NULL))
  {
    printf("Zap what monster??\n");
    return;
  }
  
  if (p == mob)
  {
    mob = mob->next;
  }
  else
  {
    while((m != NULL)  && (m->next != p))
      m = m->next;
    if (m == NULL)
    {
      printf("Error, I lost something in mobile search :(\n");
      return;
    }
    m->next = p->next;
  }

  if (tg == TRUE)
  {
    printf("A huge bolt of lightning streams from your finger tips toward %s\n",
           p->pname);
    printf("%s just looks horrified for a moment, then there is a flash of\n",
           p->name);
    printf("white flame, a loud but short screach, and a pile of ashes\n");
  }
  else
  {
    printf("You charge at %s, who gasps at you looking stunned\n",p->pname);
    printf("You hack and chop %s up into little bits, blood flying all\n",
           p->name);
    printf("over the place.\n");
  }
  nummob--;
  /* Drop all the objects */
  for (o = obj; o != NULL; o = o->next)
  {
    if ((o->ltype == CARRIED_BY) || (o->ltype == WORN_BY) 
        || (o->ltype == BOTH_BY) || (o->ltype == WIELDED_BY))
    {
      if (p == (PMOB)o->location)
      {
        if (p->loc != NULL)
          o->ltype = IN_ROOM;
        else
          o->ltype = UNKNOWN;
        o->location = p->loc;
      }
    }
  }
  FREE(p->name);
  FREE(p->pname);
  FREE(p->locname);
  FREE(p->desc);
  FREE(p->exam);
  free(p);
  modified = TRUE;
  return;
}
