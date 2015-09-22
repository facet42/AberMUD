/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    The names of every verbs in the game. lookup is used on this table     *
 *  in abered.c, and a call is made to the appropriate funtion in a switch.  *
 *  The verbs now all have a #define VERB_<name> to allow reordering without *
 *  changing abered.c as well.  The #define MUST be the verbs position in    *
 *  the table. Can hard code into abered.c if you like.                      *
 *                     DO NOT MOVE THE DIRECTION COMMANDS                    *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *verbs[] = {
	"North",		/*  0 */
	"East",			/*  1 */
	"South",		/*  2 */
	"West",			/*  3 */
	"Up",			/*  4 */
	"Down",			/*  5 */
	"Brief",		/*  6 */
	"Check",		/*  7 */
	"Drop",			/*  8 */
	"Dump",			/*  9 */
	"Examine",		/* 10 */
	"Exits",		/* 11 */
	"Get",			/* 12 */
	"Give",			/* 13 */
	"Globals",		/* 14 */
	"Goto",			/* 15 */
	"Help",			/* 16 */
	"Inventory",		/* 17 */
        "Link",			/* 18 */
	"List",			/* 19 */
	"Look",			/* 20 */
	"Put",			/* 21 */
	"Save",			/* 22 */
	"Score",		/* 23 */
	"Summon",		/* 24 */
	"Take",			/* 25 */
	"Trans",		/* 26 */
	"?",			/* 27 */
	"Lcreate",		/* 28 */
	"Mcreate",		/* 29 */
	"Ocreate",		/* 30 */
	"Ledit",		/* 31 */
	"Medit",		/* 32 */
	"Oedit",		/* 33 */
	"Lflags",		/* 34 */
	"Mflags",		/* 35 */
	"Oflags",		/* 36 */
	"Pflags",		/* 37 */
	"Sflags",		/* 38 */
	"Lstats",		/* 39 */
	"Mstats",		/* 40 */
	"Ostats",		/* 41 */
	"Lzap",			/* 42 */
	"Mzap",			/* 43 */
	"Ozap",			/* 44 */
	"Kill",                 /* 45 */
	"Define",               /* 46 */
	"Undefine",             /* 47 */
	"Wflags",               /* 48 */
	"Clone",                /* 49 */
	"Comment",		/* 50 */
	TABLE_END
};

/* The position of every verb in the table */

#define VERB_NORTH	0
#define VERB_EAST	1
#define VERB_SOUTH	2
#define VERB_WEST	3
#define VERB_UP		4
#define VERB_DOWN	5
#define VERB_BRIEF	6
#define VERB_CHECK	7
#define VERB_DROP	8
#define VERB_DUMP	9
#define VERB_EXAMINE	10
#define VERB_EXITS	11
#define VERB_GET	12
#define VERB_GIVE	13
#define VERB_GLOBALS	14
#define VERB_GOTO	15
#define VERB_HELP	16
#define VERB_INVENT	17
#define VERB_LINK	18
#define VERB_LIST	19
#define VERB_LOOK	20
#define VERB_PUT	21
#define VERB_SAVE	22
#define VERB_SCORE	23
#define VERB_SUMMON	24
#define VERB_TAKE	25
#define VERB_TRANS	26
#define VERB_HELP2	27
#define VERB_LCREATE	28
#define VERB_MCREATE	29
#define VERB_OCREATE	30
#define VERB_LEDIT	31
#define VERB_MEDIT	32
#define VERB_OEDIT	33
#define VERB_LFLAGS	34
#define VERB_MFLAGS	35
#define VERB_OFLAGS	36
#define VERB_PFLAGS	37
#define VERB_SFLAGS	38
#define VERB_LSTATS	39
#define VERB_MSTATS	40
#define VERB_OSTATS	41
#define VERB_LZAP	42
#define VERB_MZAP	43
#define VERB_OZAP	44
#define VERB_KILL       45
#define VERB_DEFINE	46
#define VERB_UNDEFINE	47
#define VERB_WFLAGS     48
#define VERB_CLONE      49
#define VERB_COMMENT	50
