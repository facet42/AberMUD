/*
 * Stub/support file for unit-testing SaveLoad.c's serialization
 * primitives in isolation. SaveLoad.c is one translation unit covering
 * everything from a single short to the whole item database, tables,
 * and BSX graphics, so it references roughly 45 other engine functions
 * in code paths these tests never exercise (item/table/BSX
 * construction, flag/class/bit name registries, ...). This file
 * satisfies the linker for those with trivial stubs.
 *
 * The one exception is PairArg(), copied verbatim from CompileTable.c
 * (see its comment there for why four words rather than two) because
 * SaveLoad.c's own SaveAction/LoadAction call it directly for the
 * transient in-memory pointer packing used by compiled table lines --
 * not exercised by the tests here, but needed to link.
 */
#include "../Source/System.h"
#include "../Source/User.h"

uintptr_t PairArg(unsigned short *x)
{
	return ((uintptr_t)x[0] << 48) | ((uintptr_t)x[1] << 32)
		| ((uintptr_t)x[2] << 16) | (uintptr_t)x[3];
}

/* ---- globals other translation units normally provide ---- */
ITEM *ItemList = NULL;
TABLE *TableList = NULL;
WLIST *WordList = NULL;
char *PBitNames[16];
char *OBitNames[16];
char *RBitNames[16];
char *CBitNames[16];
char *FlagName[512];
char *Cnd_Table[] = { "        DUMMY", NULL };

/* ---- trivial stubs: everything outside the primitives under test ---- */

int AddNLChain(ITEM *a, ITEM *b) { (void)a; (void)b; return 0; }
void AddWord(char *a, short b, short c) { (void)a; (void)b; (void)c; }
TPTR AllocComment(char *s) { (void)s; return NULL; }

char *AllocFunc(int size, char *mod, char *ver, int line, char *file)
{
	(void)mod; (void)ver; (void)line; (void)file;
	return calloc(1, (size_t)size);
}

SUB *AllocSub(ITEM *i, short key, short size) { (void)i; (void)key; (void)size; return NULL; }

/* Minimal but real (not a no-op): SaveString()/LoadString() need a
 * working TXT allocator to round-trip through for a meaningful test,
 * since the real one is a text-interning hash table well beyond what
 * these tests need. */
TPTR AllocText(char *s)
{
	TXT *t = malloc(sizeof(TXT));
	t->te_Text = malloc(strlen(s) + 1);
	strcpy(t->te_Text, s);
	t->te_Users = 1;
	t->te_Next = NULL;
	return t;
}
BSXImage *BSXAllocate(char *a, int b) { (void)a; (void)b; return NULL; }
void BSXDelete(BSXImage *i) { (void)i; }
BSXImage *BSXFindFirst(void) { return NULL; }
BSXImage *BSXFindNext(BSXImage *i) { (void)i; return NULL; }
CONTAINER *BeContainer(ITEM *i) { (void)i; return NULL; }
char *CBitName(int n) { (void)n; return ""; }
long CountItems(void) { return 0; }
TPTR QuickAllocText(char *s) { return AllocText(s); }
char *TextOf(TPTR t) { return t ? t->te_Text : ""; }
void FreeText(TPTR t) { if (t) { free(t->te_Text); free(t); } }

void ErrFunc(char *msg, char *mod, char *ver, int line, char *file)
{
	fprintf(stderr, "ErrFunc called unexpectedly: %s (%s %s:%d %s)\n",
		msg, mod, ver, line, file);
	abort();
}

SUB *FindSub(ITEM *i, short key) { (void)i; (void)key; return NULL; }
TXT *GetClassTxt(short n) { (void)n; return NULL; }
void KillEventQueue(ITEM *i) { (void)i; }
void Log(char *fmt, ...) { (void)fmt; }
int MakeGenExit(ITEM *i) { (void)i; return 0; }
CONDEXIT *MakeNLCondExit(ITEM *a, ITEM *b, short c, short d) { (void)a; (void)b; (void)c; (void)d; return NULL; }
MSGEXIT *MakeNLMsgExit(ITEM *a, ITEM *b, short c, char *d) { (void)a; (void)b; (void)c; (void)d; return NULL; }
int MakeObject(ITEM *i) { (void)i; return 0; }
int MakePlayer(ITEM *i) { (void)i; return 0; }
int MakeRoom(ITEM *i) { (void)i; return 0; }
long MasterNumber(ITEM *i) { (void)i; return 0; }
LINE *NewLine(TABLE *t, int n) { (void)t; (void)n; return NULL; }
TABLE *NewTable(int n, char *s) { (void)n; (void)s; return NULL; }
char *OBitName(int n) { (void)n; return ""; }
char *PBitName(int n) { (void)n; return ""; }
char *RBitName(int n) { (void)n; return ""; }
void SetClassName(short n, char *s) { (void)n; (void)s; }
void SetClassTxt(short n, TXT *t) { (void)n; (void)t; }
void SetFlagName(short n, char *s) { (void)n; (void)s; }
void SetHereMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetInMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetOutMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetUText(ITEM *i, int n, TPTR t) { (void)i; (void)n; (void)t; }
void SetUserFlag(ITEM *i, int n, int v) { (void)i; (void)n; (void)v; }
USERFLAG *UserFlag2Of(ITEM *i) { (void)i; return NULL; }
USERFLAG *UserFlagOf(ITEM *i) { (void)i; return NULL; }
