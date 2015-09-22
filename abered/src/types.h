/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    The type declarations for abered, including definitions for mobiles    *
 *  locations and objects. The settings for length of buffers etc, and if    *
 *  we need the Berkeley include library, containing strncasecmp and         *
 *  strcasecmp. (EQBUG) Also contains #defines for parsing mobiles and       *
 *  objects, done in mob_io.c and obj_io.c  #define ANSI_C on UNIX/ANSI      *
 *  machines                                                                 *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _abered
#define _abered         

#define CONFIG_FILE   "abered.cfg" /* Configuration file                     */

#ifndef DOS_MACHINE

#define HELP_DIR      "HELP/"     /* Help directory for UNIX                */

#else

#define HELP_DIR      "HELP\\"    /* Directory where help files are.        */

#endif

#ifndef PCC_286
#include <stdlib.h>
#endif

#ifndef EQBUG
#include <string.h>
#endif


#define TRUE		1          /* These are the meaning of life :)       */
#define FALSE		0

#define TABLE_END	(char *)(-1)

#define MAX_COM		100        /* Longest command we will accept.        */
#define MAX_BUFFER      10240      /* 10K buffer for descriptions.           */

#define UNKNOWN		0          /* Serves for objects, locations & mobs   */

#ifndef ANSI_C 
typedef unsigned char u_char;      /* For EQBUG mostly :)                    */
#endif
typedef unsigned char Bool;        /* Easy way to do booleans, smaller int   */

/* ========================== Location data types ========================== */

#define X_OBJECT	1          /* Exit types.                            */
#define X_NORMAL	2
#define X_NONE		3
#define NEXITS		6          /* Number of exits, eg n,s,e,w,u,d        */
#define NUM_LFLAGS	32         /* Max of 32 lflags.                      */

typedef struct _loc *PLOC;

typedef struct _loc {
    char *name;                    /* Unique internal name.                  */
    char *pname;                   /* Name of the location.                  */
    char *desc;                    /* Locations description.                 */
    char exit_type[NEXITS];        /* Type of exit, to room, or linked obj   */
    char *ename[NEXITS];           /* Exit names. Case when it is UNKNOWN    */
    void *exits[NEXITS];           /* array of pointers to exits.            */
    Bool lflags[NUM_LFLAGS];       /* Array of location flags.               */
    PLOC next;                     /* Next location in linked list           */ 
}LOC;

/* ============================ Object data types ========================== */

#define NUM_OFLAGS	32         /* Max number of oflags.                  */
#define NUM_WFLAGS      32         /* Max number of wflags                   */
#define MAX_STATES      4          /* Most number states allowed.            */

#define IN_ROOM		1          /* Types of locations for an object.      */
#define IN_CONTAINER	2
#define CARRIED_BY	3
#define WIELDED_BY	4
#define WORN_BY		5
#define BOTH_BY		6

#define O_NAME		0          /* Set of values to match what lookup     */
#define O_PNAME		1          /* Would return when looking in the       */
#define O_LOC		2          /* table of object things.                */
#define O_DESC		3
#define O_END		4
#define O_ANAME		5
#define O_OFLAGS	6
#define O_ARMOR		7
#define O_DAMAGE	8
#define O_MAXS		9
#define O_STATE		10
#define O_VALUE		11
#define O_SIZE		12
#define O_WEIGHT	13
#define O_EXAM		14
#define O_LINK		15
#define O_COUNT		16
#define O_WFLAGS        17
#define O_WLEVEL        18

typedef struct _obj *POBJ;

typedef struct _obj {
    char *name;                    /* Object name. Unique to zone.           */
    char *pname;                   /* Name players will see.                 */
    char *altname;                 /* Alternative name for object.           */
    char *desc[MAX_STATES];        /* Description for each possible state    */
    char *examine;                 /* Pointer to examine text.               */
    char *lname;                   /* Name of location read in.              */
    char ltype;                    /* CARRIED, IN_ROOM etc.                  */
    char *link;                    /* If linked, to what                     */
    void *location;                /* Pointer to actual loc/obj/mob          */
    int  number;                   /* Unique number. Unimportant really.     */
    int  damage;                   /* Damage. (If weapon)                    */
    int  armor;                    /* Armor value. (If armor)                */
    int  state;                    /* Current and starting state             */
    int  maxstate;                 /* Maximum state.                         */
    int  value;                    /* Base Value when pitted.                */
    int  size;                     /* How big is it.                         */
    int  weight;                   /* How much does it weigh                 */
    int  counter;                  /* General purpose.                       */
    Bool oflags[NUM_OFLAGS];       /* Array of flags                         */

#ifndef _NO_WFLAGS
    int  wlevel;                   /* The wear level of an object.           */
    Bool wflags[NUM_WFLAGS];       /* Array of flags                         */
#endif

    POBJ next;                     /* Next object in list.                   */
}OBJ;

/* ============================ Mobile data types ========================== */

#define NUM_PFLAGS	64         /* Max number of pflags allowed.          */
#define NUM_SFLAGS      32         /* Max number of sflags                   */
#define NUM_MFLAGS      64         /* Max number of mobile flags.            */

#define MOB_NAME	0          /* Values returned by lookup when         */
#define MOB_PNAME	1          /* The table Mob_tab[] in mob_io.c  This  */
#define MOB_LOC         2          /* controls the switch statement and      */
#define MOB_DESC	3          /* Parsing of mobile data.                */
#define MOB_END		4
#define MOB_STR		5
#define MOB_DAM 	6
#define MOB_AGG 	7
#define MOB_ARMOR 	8
#define MOB_SPEED 	9
#define MOB_PFLAGS 	10
#define MOB_SFLAGS 	11
#define MOB_MFLAGS 	12
#define MOB_EXAM 	13

typedef struct _mob *PMOB;         /* Pointer to a mobile data type          */

typedef struct _mob {
    char *name;                    /* Mobiles internal name.                 */
    char *pname;                   /* Name the players see.                  */
    char *locname;                 /* Name of loc, if loc = NULL invalid loc */
    PLOC loc;                      /* Pointer to location of the mobile      */
    int  level;                    /* Mobile level. May not be used.         */
    int  hp;                       /* Mobiles strength. (HitPoints)          */
    int  armor;                    /* Natural AC of the mobile               */
    int  damage;                   /* Natural damage done by mobile.         */
    int  agg;                      /* Aggression of the mob as a %           */
    int  speed;                    /* How fast the mobile walks.             */
    Bool pflags[NUM_PFLAGS];       /* Pflags (privilleged bits)              */
    Bool sflags[NUM_SFLAGS];       /* Slfags are condotions, eg female.      */
    Bool mflags[NUM_MFLAGS];       /* Mflags are special for mobiles.        */
    char *desc;                    /* Mobiles description.                   */
    char *exam;                    /* Whar supposed to see when examined     */
    PMOB next; 
}MOB;

#endif
