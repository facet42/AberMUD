/*
 * Stub/support file for the full parser-to-VM integration test: typed
 * command text -> GetVerb()/UserAction()/ParseArgs()/GetThing() (real
 * Parser.c/TableDriver.c) -> FindAnItem() resolving real room/inventory
 * items by adjective+noun -> a table compiled by the real CompileTable.c
 * -> executed by the real CondCode.c/ActionCode.c/TableDriver.c VM.
 *
 * Same real-vs-stub reasoning as vm_stubs.c/compile_stubs.c throughout
 * (see their comments); this file is their near-duplicate, again minus
 * the symbols Parser.c/CompileTable.c supply for real (WordPtr,
 * GetContext, SetItData, GetRestOfInput, GetOrd, GetPrep, GetThing,
 * GetVerb, PairArg).
 *
 * The one thing this file needs beyond compile_stubs.c: FindIn()/
 * NextIn() (and the WordMatch() helper they share) are upgraded from
 * always-NULL stubs to real implementations, because FindAnItem() --
 * the function that turns a parsed adjective+noun into an actual
 * ITEM* by searching the player's room and inventory -- is exactly
 * what this test exists to exercise. A stub here would make the
 * "does typed text resolve to the right real item" question
 * untestable, which is the whole point.
 */
#include "../Source/System.h"
#include "../Source/User.h"
#include <setjmp.h>

/* ---- globals other translation units normally provide ---- */
USER UserList[MAXUSER];
jmp_buf Oops;
short post_boot = 1;

/* ---- faithful copies of small, self-contained real implementations ---- */
/* (PairArg() itself is not stubbed here -- CompileTable.c, linked for
 * real, already provides it.) */

/* Verbatim from System.c (minus the CHECK_ITEM debug hook, which is
 * compiled out by default anyway). */
SUB *FindSub(ITEM *item, register short key)
{
	ITEM *b = NULL;
	register SUB *a = item->it_Properties;
	while (a) {
		if (a->pr_Key == key)
			return (a);
		if (a->pr_Key == KEY_INHERIT)
			b = ((INHERIT *)(a))->in_Master;
		a = a->pr_Next;
	}
	if (!post_boot)
		return (NULL);
	if (b) {
		a = b->it_Properties;
		while (a) {
			if (a->pr_Key == key)
				return (a);
			a = a->pr_Next;
		}
	}
	return (NULL);
}

/* Verbatim from SysSupport.c: thin FindSub() wrappers. */
PLAYER *PlayerOf(ITEM *x) { return ((PLAYER *)FindSub(x, KEY_PLAYER)); }
OBJECT *ObjectOf(ITEM *x) { return ((OBJECT *)FindSub(x, KEY_OBJECT)); }
ROOM *RoomOf(ITEM *x) { return ((ROOM *)FindSub(x, KEY_ROOM)); }
CONTAINER *ContainerOf(ITEM *x) { return ((CONTAINER *)FindSub(x, KEY_CONTAINER)); }

int UserOf(ITEM *x)
{
	PLAYER *p = PlayerOf(x);
	if (p == NULL)
		return (-1);
	return (p->pl_UserKey);
}

int IsRoom(ITEM *i) { return (RoomOf(i) != NULL); }
int IsPlayer(ITEM *i) { return (PlayerOf(i) != NULL); }
int IsObject(ITEM *i) { return (ObjectOf(i) != NULL); }
int IsUser(ITEM *x) { return (UserOf(x) != -1); }

/* Verbatim from System.c. */
int Contains(register ITEM *a, register ITEM *b)
{
	int ct = 32;
	while (O_PARENT(b) && ct--) {
		if (O_PARENT(b) == a)
			return (1);
		b = O_PARENT(b);
	}
	return (0);
}

/* Verbatim from System.c. */
int CanSee(short pe, ITEM *it)
{
	if (it->it_Perception > pe)
		return (0);
	return (1);
}

/* Verbatim from System.c. */
void SetState(ITEM *x, short v) { x->it_State = v; }

/* Simplified from SysSupport.c: see vm_stubs.c's comment on the same
 * simplification. */
short LevelOf(ITEM *x)
{
	PLAYER *p = PlayerOf(x);
	if (p == NULL)
		return (0);
	return (p->pl_Level);
}

int ArchWizard(ITEM *i) { (void)i; return 0; }

/* ---- item-tree linking (System.c) ---- */

int UnlinkItem(register ITEM *x)
{
	register ITEM *a;
	if (O_FREE(x))
		return (0);
	if (O_CHILDREN(O_PARENT(x)) == x) {
		O_CHILDREN(O_PARENT(x)) = O_NEXT(x);
		O_PARENT(x) = NULL;
		O_NEXT(x) = NULL;
		return (0);
	}
	a = O_CHILDREN(O_PARENT(x));
	if (a == NULL)
		return (0);
	while (O_NEXT(a)) {
		if (O_NEXT(a) == x) {
			O_NEXT(a) = O_NEXT(x);
			O_PARENT(x) = NULL;
			O_NEXT(x) = NULL;
			return (0);
		}
		a = O_NEXT(a);
	}
	return (0);
}

int LinkItem(ITEM *a, ITEM *b)
{
	if (!O_FREE(a))
		return (-1);
	O_PARENT(a) = b;
	if (b) {
		O_NEXT(a) = O_CHILDREN(b);
		O_CHILDREN(b) = a;
	} else {
		O_NEXT(a) = NULL;
	}
	return (0);
}

void XPlace(ITEM *x, ITEM *y)
{
	if (!O_FREE(x))
		UnlinkItem(x);
	LinkItem(x, y);
}

void Place(ITEM *x, ITEM *y)
{
	if (IsObject(x))
		ObjectOf(x)->ob_Flags &= ~OB_WORN;
	XPlace(x, y);
}

int WeighUp(ITEM *i) { (void)i; return 0; }
int CanPlace(ITEM *a, ITEM *b) { (void)a; (void)b; return 0; }

/* Verbatim from System.c: what FindIn()/NextIn() below match on. */
int WordMatch(ITEM *i, short a, short n)
{
	if ((a == -1) && (n == O_NOUN(i)))
		return (1);
	if ((a == O_ADJECTIVE(i)) && (n == O_NOUN(i)))
		return (1);
	return (0);
}

/* Verbatim from System.c: linear scan of a's children for the first/
 * next one matching adjective a and noun n, visible at perception pe. */
ITEM *FindIn(short pe, ITEM *i, short a, short n)
{
	if (i == NULL)
		return (NULL);
	i = O_CHILDREN(i);
	while (i) {
		if (WordMatch(i, a, n) && CanSee(pe, i))
			return (i);
		i = O_NEXT(i);
	}
	return (NULL);
}

ITEM *NextIn(short pe, register ITEM *i, short a, short n)
{
	i = O_NEXT(i);
	while (i) {
		if (WordMatch(i, a, n) && CanSee(pe, i))
			return (i);
		i = O_NEXT(i);
	}
	return (NULL);
}

/* Always returns the same fixed item regardless of adj/noun/class,
 * simulating "there is exactly one item in the game matching this
 * description" -- enough for a test to compile a literal (non-$1/$2/
 * $ME-sentinel) item reference through EncodeItem(). Not static: tests
 * set its it_Adjective/it_Noun to match whatever word codes they
 * registered, and can check that ArgItem() decodes back this exact
 * pointer at execution time. */
ITEM CompileTestFakeMasterItem;
ITEM *FindMaster(short a, short b, short c) { (void)a; (void)b; (void)c; return &CompileTestFakeMasterItem; }
ITEM *NextMaster(short a, ITEM *b, short c, short d) { (void)a; (void)b; (void)c; (void)d; return NULL; }
int IsUnique(short a, short b, short c) { (void)a; (void)b; (void)c; return 1; }
int ItemNumber(short a, ITEM *b) { (void)a; (void)b; return 0; }

/* ---- everything else: trivial stubs for engine areas out of scope ---- */

void Act_BSXObject(void) {}
void Act_BSXScene(void) {}
void Act_CDaemon(void) {}
void Act_DelExit(void) {}
void Act_DoorExit(void) {}
void Act_ForkDump(void) {}
void Act_Getvis(void) {}
void Act_RwhoDeclareAlive(void) {}
void Act_RwhoDeclareDown(void) {}
void Act_RwhoDeclareUp(void) {}
void Act_RwhoLogin(void) {}
void Act_RwhoLogout(void) {}
void Act_SetExit(void) {}
void Act_SetI(void) {}
void Act_SetSuper(void) {}
void Act_WhereTo(void) {}

void AddEvent(unsigned long a, short b) { (void)a; (void)b; }
void AllDaemon(int a, int b, int c) { (void)a; (void)b; (void)c; }

char *AllocFunc(int size, char *mod, char *ver, int line, char *file)
{
	(void)mod; (void)ver; (void)line; (void)file;
	return calloc(1, (size_t)size);
}

TPTR AllocComment(char *s) { (void)s; return NULL; }
void FreeComment(TPTR t) { (void)t; }
char *GetClassName(short n) { (void)n; return ""; }
int GetFlagByName(char *s) { (void)s; return -1; }
char *GetFlagName(short n) { (void)n; return ""; }
short WhichClass(char *s) { (void)s; return -1; }

TPTR AllocText(char *s) { (void)s; return NULL; }
void Broadcast(char *msg, int flag) { (void)msg; (void)flag; }
char *CNameOf(ITEM *i) { (void)i; return ""; }
int CanGoto(ITEM *a, ITEM *b) { (void)a; (void)b; return 0; }
void ChainDaemon(ITEM *a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
ITEM *Clone_Item(ITEM *a, short b) { (void)a; (void)b; return NULL; }
void Cmd_Exits(ITEM *a, ITEM *b) { (void)a; (void)b; }
void Cmd_Look(ITEM *a) { (void)a; }
void Cmd_MoveDirn(ITEM *a, int b) { (void)a; (void)b; }

int Cnd_Delete(void) { return 0; }
int Cnd_FLoad(void) { return 0; }
int Cnd_FSave(void) { return 0; }
int Cnd_GetSuper(void) { return 0; }
int Cnd_Member(void) { return 0; }
int Cnd_ProcDaemon(void) { return 0; }
int Cnd_ProcObject(void) { return 0; }
int Cnd_ProcSubject(void) { return 0; }
int Cnd_ULoad(void) { return 0; }
int Cnd_USave(void) { return 0; }

void DescribeItem(ITEM *a, ITEM *b) { (void)a; (void)b; }
int Disintegrate(ITEM *i) { (void)i; return 0; }

void DoesAction(ITEM *user, int flag, char *msg, char *p1, char *p2, char *p3, char *p4, char *p5, char *p6)
{
	(void)user; (void)flag; (void)msg;
	(void)p1; (void)p2; (void)p3; (void)p4; (void)p5; (void)p6;
}

void DoesTo(ITEM *user, int flag, ITEM *thing, char *msg, char *p1, char *p2, char *p3, char *p4, char *p5, char *p6)
{
	(void)user; (void)flag; (void)thing; (void)msg;
	(void)p1; (void)p2; (void)p3; (void)p4; (void)p5; (void)p6;
}

void DoesToPlayer(ITEM *user, int flag, ITEM *thing, char *msg, char *p1, char *p2, char *p3, char *p4, char *p5, char *p6)
{
	(void)user; (void)flag; (void)thing; (void)msg;
	(void)p1; (void)p2; (void)p3; (void)p4; (void)p5; (void)p6;
}

void ErrFunc(char *msg, char *mod, char *ver, int line, char *file)
{
	fprintf(stderr, "ErrFunc called unexpectedly: %s (%s %s:%d %s)\n",
		msg, mod, ver, line, file);
	abort();
}

char *ExitName(int n) { (void)n; return ""; }
void ExitUser(unsigned int u) { (void)u; }
ITEM *FindInByClass(short a, ITEM *b, short c) { (void)a; (void)b; (void)c; return NULL; }
void FreeText(TPTR t) { (void)t; }
char *GetHereMsg(ITEM *i) { (void)i; return ""; }
char *GetInMsg(ITEM *i) { (void)i; return ""; }
ITEM *GetNextPointer(void) { return NULL; }
char *GetOutMsg(ITEM *i) { (void)i; return ""; }
TPTR GetUText(ITEM *i, int n) { (void)i; (void)n; return NULL; }
int GetUserFlag(ITEM *i, int n) { (void)i; (void)n; return 0; }
ITEM *GetUserItem(ITEM *i, int n) { (void)i; (void)n; return NULL; }
void HDaemon(ITEM *a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
int IsDarkFor(ITEM *i) { (void)i; return 0; }
int LoadPersona(char *a, UFF *b) { (void)a; (void)b; return -1; }
void LockItem(ITEM *i) { (void)i; }
void Log(char *fmt, ...) { (void)fmt; }
char *NameOf(ITEM *i) { (void)i; return ""; }
int Name_Got(int a, char *b) { (void)a; (void)b; return 0; }
ITEM *NextInByClass(short a, ITEM *b, short c) { (void)a; (void)b; (void)c; return NULL; }
void PermitInput(int a) { (void)a; }
void RemoveUser(unsigned int u) { (void)u; }
void RunDaemon(ITEM *a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
int SaveNewPersona(UFF *u) { (void)u; return -1; }
int SavePersona(UFF *u, int n) { (void)u; (void)n; return -1; }

/* Matches SysSupport.c's real K&R-style signature (prototype.h declares
 * this the same old way: "void SendItem();", unspecified args) rather
 * than a true (...) varargs definition, which the two are not
 * interchangeable for. */
void SendItem(ITEM *x, char *a, char *b, char *c, char *d, char *e, char *f)
{
	(void)x; (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
}

int SendNPacket(PORT *a, short b, short c, short d, short e, short f)
{
	(void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
	return 0;
}

int SendTPacket(PORT *a, short b, char *c) { (void)a; (void)b; (void)c; return 0; }
void SendUser(int u, char *fmt, ...) { (void)u; (void)fmt; }
void SetHereMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetInMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetName(ITEM *i, char *s) { (void)i; (void)s; }
void SetNextPointer(ITEM *i) { (void)i; }
void SetOutMsg(ITEM *i, char *s) { (void)i; (void)s; }
void SetPrompt(ITEM *i, char *s) { (void)i; (void)s; }
void SetUText(ITEM *i, int n, TPTR t) { (void)i; (void)n; (void)t; }
void SetUserFlag(ITEM *i, int n, int v) { (void)i; (void)n; (void)v; }
void SetUserItem(ITEM *i, int n, ITEM *v) { (void)i; (void)n; (void)v; }
int StartSnoop(ITEM *a, ITEM *b, short c) { (void)a; (void)b; (void)c; return 0; }
void StopAllSnoops(ITEM *i) { (void)i; }
void StopSnoopOn(ITEM *a, ITEM *b) { (void)a; (void)b; }
void SynchChain(ITEM *i) { (void)i; }
char *TextOf(TPTR t) { (void)t; return ""; }
void TreeDaemon(ITEM *a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
void UnlockItem(ITEM *i) { (void)i; }
void UserVector(short a, short b, short c, ITEM *d, ITEM *e, TPTR f) { (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; }
int ValidItem(ITEM *i) { (void)i; return 1; }
