/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *    The variables that are truely global. They are known by every other    *
 *  source file. Most of these (if not all) are declared in abered.c and     *
 *  that is the only .c file that does not include this one.                 *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern PLOC loc;
extern POBJ obj;
extern PMOB mob;
extern PMOB me;
extern int  objnum;
extern int  numobj;
extern int  nummob;
extern int  numloc;
extern int  text_lines;
extern FILE *ferr;
extern char cmdln[MAX_COM+1];
extern char *cmd;
extern char *arg[3];
extern char *pnt;
extern char *comment;
extern PLOC myloc;
extern Bool brief;
extern Bool o_counters;
extern Bool verbose;
extern char fzn[13];
extern char zone[16];
extern Bool quit;
extern Bool modified;
extern char *o_flags[NUM_OFLAGS + 1];
extern char *p_flags[NUM_PFLAGS + 1];
extern char *m_flags[NUM_MFLAGS + 1];
extern char *s_flags[NUM_SFLAGS + 1];
extern char *l_flags[NUM_LFLAGS + 1];

#ifndef _NO_WFLAGS
extern Bool wear_flags;
extern char *w_flags[NUM_WFLAGS + 1];
#endif
