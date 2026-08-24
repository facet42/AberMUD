/*
 * Parser.c and FlagName.c are single translation units that also contain
 * code paths (AddWord, GetThing, SetBitName, ...) this test binary never
 * exercises, but the linker still needs every symbol they reference
 * resolved. These stubs satisfy that without pulling in the rest of the
 * engine (sockets, game state, save files) just to unit test the pure
 * parsing/lookup helpers.
 */
#include "../Source/System.h"

void ErrFunc(char *msg, char *mod, char *ver, int line, char *file)
{
	fprintf(stderr, "ErrFunc called unexpectedly: %s (%s %s:%d %s)\n",
		msg, mod, ver, line, file);
	abort();
}

char *AllocFunc(int size, char *mod, char *ver, int line, char *file)
{
	(void)mod; (void)ver; (void)line; (void)file;
	return calloc(1, (size_t)size);
}

PLAYER *PlayerOf(ITEM *i) { (void)i; return NULL; }
OBJECT *ObjectOf(ITEM *i) { (void)i; return NULL; }
int IsRoom(ITEM *i) { (void)i; return 0; }
int IsPlayer(ITEM *i) { (void)i; return 0; }
int IsObject(ITEM *i) { (void)i; return 0; }
ITEM *Me(void) { return NULL; }

void SendItemDirect(ITEM *i, char *s) { (void)i; (void)s; }

/* Matches SysSupport.c's real K&R-style signature (prototype.h declares
 * this the same old way: "void SendItem();", unspecified args) rather
 * than a true (...) varargs definition, which the two are not
 * interchangeable for. */
void SendItem(ITEM *x, char *a, char *b, char *c, char *d, char *e, char *f)
{
	(void)x; (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
}
