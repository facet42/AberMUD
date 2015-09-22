/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ABERED (AberMUD Dirt3 editor)                       *
 *                                                                           *
 *    The prototypes for all the source files.                               *
 *                                                                           *
 *    Defining ANSI_C will get you ansi_c prototype declarations, otherwise  *
 *  it will use the default OLD C style prototypes.                          *
 *                                                                           *
 *           Copyright (C) 1993 James Willie. All rights reseved.            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ANSI_C

/* ================================= obj.c ================================= */

char *oloc_name(o);                       /* POBJ o. Returns name of loc..   */
POBJ fobject(s);                          /* char *s Ret: Object named s     */
POBJ fobname(s);                          /* char *s. find obj. by name      */
POBJ fobpname(s);                         /* char *s. find obj. by pname     */
POBJ fobaname(s);                         /* char *s. find obj. by altname   */
POBJ fobnumber(s);                        /* char *s. find obj. by number    */
void clone_obj(s, t);
void drop(s);                             /* char *s. Drop obj s or all      */
void fix_obj(o);                          /* POBJ o. fix obj. locs etc       */
void fix_objs();                          /* call fix_obj on all objects     */
void inventory();                         /* List objects with ltype UNKNOWN */
void link_door(s,t);                      /* char *s, *t. Links s and t      */
void list_obj();
void ocreate();
void oedit();
void oflags();
void wflags();
void ostat(s);
void ozap(s);                             /* char *s. Destroy object s       */

/* =============================== obj_io.c ================================ */

POBJ read_dirt_obj(f);                    /* FILE *f                         */
POBJ read_dirt_objects(f);                /* FILE *f                         */
void init_obj(n,o);                       /* char *n, POBJ o                 */
void write_dirt_obj(f,o);                 /* FILE *f, POBJ o                 */
void write_oflags(f,o);                   /* FILE *f, POBJ o                 */
void write_wflags(f,o);                   /* FILE *f, POBJ o                 */

/* =============================== loc_io.c ================================ */

PLOC get_dirt_loc(f);
PLOC load_dirt_locs(f);
int  dir2num(s);
void write_dirt_loc(l,f);

/* ================================= loc.c ================================= */

Bool redit_com();
PLOC flbname(s);
void clone_loc(s, t);                 /* char *s, *t. clones a loc           */
void create_exit(l,x,s);
void create_loc();
void delete_exit(l,x);
void destroy_loc();
void dodirn(x);
void do_destroy_loc(l);
void exitcom();
void fix_exit(l);
void fix_exits();
void gotocom();
void lflags();
void list_loc();
void lookin(l);
void rstatscom();

/* ================================= mob.c ================================= */

void clone_mob(s,t);
void fix_mobs();
void list_mob();
void mcreate(s);                          /* Create a new mob called char *s */
void mset();
void mstat(s);
void mob_flags(n);
void mzap(s,tg);                          /* destroys a mobile named char *s */
PMOB fmobile(s);

/* =============================== mob_io.c ================================ */

PMOB read_dirt_mob(f);                    /* Read 1 mob from FILE *f         */
PMOB read_dirt_mobile(f);                 /* Read seq.. of mobs into list    */
void init_mob(m,n);                       /* Initialise new mob m, name n    */
void write_dirt_mob(m,f);                 /* Write list of mobs, m, to f     */

/* =============================== abered.c ================================ */

void check();                             /* Look for errors in zone         */
void comments();
void define(flg, tab);                    /* make flg in table tab           */
void examine(s);                          /* examine mob/obj char *s         */
void free_args();
void game_com(s);                         /* char *s. Do command line s      */
void get_command();
void globals();
void helpcom();
void main(argc, argv);
void parse_com(quiet);                    /* Bool quiet. quiet = game_com    */
void not_imp();
void set_args(a,b,c);
void undefine(flg, tab);                  /* Remove a flag.                  */

/* ================================== io.c ================================= */

Bool read_truth(f);                       /* Reads a word, TRUE or FALSE     */
void new_flags(f,table);                  /* FILE *f, char **table           */
void save_dirt();
void load_dirt();
void load_config();
void save_glob();
void read_comment(s, f);

/* ================================= utils.c =============================== */

int  lookup(table,entry);    /* char **table, char *entry. Ret index or -1   */
int  get_int(F,status);      /* FILE *F. Reads next int from F               */
int  get_word(s,f);          /* char *s, FILE *f. reads 1 word into s        */
char *btos(t);               /* Bool t. Boolean to string :)                 */
char *get_text(f,t);         /* FILE *f, char t. Read text till we read a t  */
char *get_arg();             /* Return the next com-line arg. after arg[0-2] */
void create_flag(flag,table);/* Add flag to char **table IF possible         */
void fix_string(t);          /* char *t. Removes trailing \r\n               */
void gint_error(s,n,o);
void list_flags(table);      /* char **table, list contents of table         */
void write_string(s,f);      /* char *s, FILE *f. printf buffers IO to much  */

#ifdef EQBUG                 /* Berkeley include library functions           */
int  strcasecmp(s1, s2);
int  strncasecmp(s1, s2, n);
#endif


#else

/* ================================= obj.c ================================= */

char *oloc_name(POBJ o);                  /* POBJ o. Returns name of loc..   */
POBJ fobject(char *s);                    /* char *s Ret: Object named s     */
POBJ fobname(char *s);                    /* char *s. find obj. by name      */
POBJ fobpname(char *s);                   /* char *s. find obj. by pname     */
POBJ fobaname(char *s);                   /* char *s. find obj. by altname   */
POBJ fobnumber(char *s);                  /* char *s. find obj. by number    */
void clone_obj(char *s, char *t);
void drop(char *s);                       /* char *s. Drop obj s or all      */
void fix_obj(POBJ o);                     /* POBJ o. fix obj. locs etc       */
void fix_objs();                          /* call fix_obj on all objects     */
void get(char *s);
void give(char *s, char *t);              /* give s to t                     */
void inventory();                         /* List objects with ltype UNKNOWN */
void link_door(char *s,char *t);          /* char *s, *t. Links s and t      */
void list_obj();
void ocreate();
void oedit();
void oflags();
void wflags();
void ostat(char *s);
void ozap(char *s);                       /* char *s. Destroy object s       */
void put(char *s, char *t, char *z);

/* =============================== obj_io.c ================================ */

POBJ read_dirt_obj(FILE *f);        
POBJ read_dirt_objects(FILE *f);   
void init_obj(char *n, POBJ o);         
void write_dirt_obj(FILE *f, POBJ o);  
void write_oflags(FILE *f, POBJ o);   
void write_wflags(FILE *f, POBJ o);   

/* =============================== loc_io.c ================================ */

PLOC get_dirt_loc(FILE *f);
PLOC load_dirt_locs(FILE *f);
int  dir2num(char s);
void write_dirt_loc(PLOC l,FILE *f);

/* ================================= loc.c ================================= */

Bool redit_com();
PLOC flbname(char *s);
void clone_loc(char *s, char *t);
void create_exit(PLOC l,int x,char *s);
void create_loc();
void delete_exit(PLOC l,int x);
void destroy_loc();
void dodirn(int x);
void do_destroy_loc(PLOC l);
void exitcom();
void fix_exit(PLOC l);
void fix_exits();
void gotocom();
void lflags();
void list_loc();
void lookin(PLOC l);
void rstatscom();

/* ================================= mob.c ================================= */

void clone_mob(char *s, char *t);
void fix_mobs();
void list_mob();
void mcreate(char *s);                    /* Create a new mob called char *s */
void mset();
void mstat(char *s);
void mob_flags(int n);
void mzap(char *s,int tg);                /* destroys a mobile named char *s */
PMOB fmobile(char *s);

/* =============================== mob_io.c ================================ */

PMOB read_dirt_mob(FILE *f);              /* Read 1 mob from FILE *f         */
PMOB read_dirt_mobiles(FILE *f);          /* Read seq.. of mobs into list    */
void init_mob(PMOB m, char *n);           /* Initialise new mob m, name n    */
void write_dirt_mob(PMOB m,FILE *f);      /* Write list of mobs, m, to f     */

/* =============================== abered.c ================================ */

void check();                             /* Look for errors in zone         */
void comments();
void define(char *flg, char *tab);        /* make flg in table tab           */
void examine(char *s);                    /* examine mob/obj char *s         */
void free_args();
void game_com(char *s);                   /* char *s. Do command line s      */
void get_command();
void globals();
void helpcom();
void parse_com(Bool quiet);               /* Bool quiet. quiet = game_com    */
void not_imp();
void set_args(char *a, char *b, char *c);
void undefine(char *flg, char *tab);      /* Remove a flag.                  */
void summon(char *s);
void my_pause(void);
void usage(int x);

/* ================================== io.c ================================= */

Bool read_truth(FILE *f);                 /* Reads a word, TRUE or FALSE     */
void new_flags(FILE *f, char **table);    /* FILE *f, char **table           */
void save_dirt();
void load_dirt();
void load_config();
void save_glob();
void write_config_flags(FILE *, char **);
void read_comment(char *s, FILE *f);

/* ================================= utils.c =============================== */

int  lookup(char **table,char *entry);  
int  get_int(FILE *F,int *status);/* FILE *F. Reads next int from F           */
int  get_word(char *s,FILE *f);  /* char *s, FILE *f. reads 1 word into s    */
char *btos(Bool t);               /* Bool t. Boolean to string :)            */
char *get_text(FILE *f,char t);  
char *get_arg();             /* Return the next com-line arg. after arg[0-2] */
void create_flag(char *flag,char **table);
void fix_string(char *t);         
void gint_error(int s, char *n, char *o);
void list_flags(char **table);   
void write_string(char *s,FILE *f);

#ifdef EQBUG                 /* Berkeley include library functions           */
int  strcasecmp(char *s1, char *s2);
int  strncasecmp(char *s1, char *s2, int n);
#endif

#ifdef IBM_MLJ_EDIT    /* Include mlj_edit function for desc editing.        */
#include "mlj_edit.h"
#endif
#endif /* ifndef ANSI_C */

#ifdef _NO_WFLAGS
void no_wflags();
#endif

