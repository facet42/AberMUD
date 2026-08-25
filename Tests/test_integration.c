/*
 * Full parser-to-VM integration tests: raw typed command text, through
 * the real GetVerb()/UserAction()/ParseArgs()/GetThing()/FindAnItem()
 * chain (Parser.c/TableDriver.c), resolving real room/inventory items,
 * dispatching to a table compiled by the real CompileTable.c, and
 * executed by the real CondCode.c/ActionCode.c/TableDriver.c VM --
 * closing the gap between "words a player typed" and "a table rule
 * ran", which the narrower test_vm.c/test_compile.c suites each cover
 * only one side of.
 *
 * See integration_stubs.c for what is faked out and why (mainly: real
 * FindIn()/NextIn()/WordMatch() are needed here specifically, unlike
 * the other suites, because resolving typed words to a real item is
 * the point of this file).
 */
#include "../Source/System.h"
#include "../Source/User.h"
#include "test_framework.h"

extern ITEM *Item1, *Item2;
extern char *WordPtr;

static void reset_item(ITEM *i)
{
	memset(i, 0, sizeof(ITEM));
	i->it_Adjective = -1;
	i->it_Noun = -1;
}

static void link_item(ITEM *item, ITEM *parent)
{
	item->it_Parent = parent;
	if (parent) {
		item->it_Next = parent->it_Children;
		parent->it_Children = item;
	} else {
		item->it_Next = NULL;
	}
}

/* Compiles `src` into `line`, asserting it compiled cleanly -- every
 * test here is set up so compilation itself is not what's under test. */
static void CompileOK(char *src, LINE *line)
{
	ITEM dummy_actor;
	reset_item(&dummy_actor);
	memset(line, 0, sizeof(LINE));
	LoadLineBuffer(src);
	TEST_CHECK_EQ_INT(EncodeEntry(&dummy_actor, line), 0);
}

/* Registers `line` as player's actor table under a fresh table number
 * (each call uses a new number, so tests never risk colliding with an
 * earlier test's table of the same number still being findable). */
static void SetActorTable(ITEM *player, LINE *line)
{
	static int next_table_number = 100;
	TABLE *t = NewTable(next_table_number, "test");
	t->tb_First = line;
	player->it_ActorTable = (short)next_table_number;
	next_table_number++;
}

static void test_full_pipeline_get_red_ball(void)
{
	ITEM room, player, ball;
	LINE line;
	char cmd[] = "get red ball";

	AddWord("get", 5, WD_VERB);
	AddWord("ball", 8, WD_NOUN);
	AddWord("red", 3, WD_ADJ);

	reset_item(&room); reset_item(&player); reset_item(&ball);
	ball.it_Adjective = 3; ball.it_Noun = 8;
	link_item(&player, &room);
	link_item(&ball, &room);
	SetMe(&player);
	SetPersona(0);

	CompileOK("get ANY ANY LET 5 1", &line);
	SetActorTable(&player, &line);

	WipeFlags();
	WordPtr = cmd;
	{
		int v = GetVerb();
		TEST_CHECK_EQ_INT(v, 5);
		UserAction(&player, v);
	}
	/* The real payoff: GetThing()/FindAnItem() resolved "red ball" to
	 * this exact room item, not a copy or a different one. */
	TEST_CHECK(Item1 == &ball);
	TEST_CHECK_EQ_INT(GetFlag(5), 1);
}

static void test_item_in_inventory_is_also_found(void)
{
	ITEM room, player, key;
	LINE line;
	char cmd[] = "get brass key";

	AddWord("get", 5, WD_VERB);
	AddWord("key", 9, WD_NOUN);
	AddWord("brass", 4, WD_ADJ);

	reset_item(&room); reset_item(&player); reset_item(&key);
	key.it_Adjective = 4; key.it_Noun = 9;
	link_item(&player, &room);
	link_item(&key, &player);	/* carried, not in the room */
	SetMe(&player);
	SetPersona(0);

	CompileOK("get ANY ANY LET 5 1", &line);
	SetActorTable(&player, &line);

	WipeFlags();
	WordPtr = cmd;
	UserAction(&player, GetVerb());
	TEST_CHECK(Item1 == &key);
	TEST_CHECK_EQ_INT(GetFlag(5), 1);
}

static void test_unregistered_verb_is_not_recognized(void)
{
	char cmd[] = "xyzzy the ball";
	WordPtr = cmd;
	TEST_CHECK_EQ_INT(GetVerb(), -1);
}

static void test_nonexistent_item_leaves_item1_null(void)
{
	ITEM room, player, ball;
	LINE line;
	char cmd[] = "get green ball";

	AddWord("get", 5, WD_VERB);
	AddWord("ball", 8, WD_NOUN);
	AddWord("red", 3, WD_ADJ);
	AddWord("green", 11, WD_ADJ);

	reset_item(&room); reset_item(&player); reset_item(&ball);
	ball.it_Adjective = 3; ball.it_Noun = 8;	/* only a *red* ball exists */
	link_item(&player, &room);
	link_item(&ball, &room);
	SetMe(&player);
	SetPersona(0);

	CompileOK("get ANY ANY LET 5 1", &line);
	SetActorTable(&player, &line);

	WipeFlags();
	WordPtr = cmd;	/* asks for the green one, which doesn't exist */
	UserAction(&player, GetVerb());
	TEST_CHECK_NULL(Item1);
	/* The actor table rule still runs (its noun slots are ANY/ANY, so
	 * verb matching alone is enough) -- it is specifically Item1 that
	 * reflects "no such item", not whether the table fired at all. */
	TEST_CHECK_EQ_INT(GetFlag(5), 1);
}

static void test_subjecttable_done_stops_actortable_fallthrough(void)
{
	ITEM room, player, ball;
	LINE subject_line, actor_line;
	TABLE subject_table;
	char cmd[] = "get red ball";

	AddWord("get", 5, WD_VERB);
	AddWord("ball", 8, WD_NOUN);
	AddWord("red", 3, WD_ADJ);

	reset_item(&room); reset_item(&player); reset_item(&ball);
	ball.it_Adjective = 3; ball.it_Noun = 8;
	link_item(&player, &room);
	link_item(&ball, &room);
	SetMe(&player);
	SetPersona(0);

	/* DONE (CondCode.c's Cnd_Table entry list) makes RunLine/ExecTable
	 * return 1, which UserAction() (TableDriver.c) reads as "this
	 * table fully handled the command" and returns without ever
	 * looking at the player's actor table. */
	CompileOK("get ANY ANY LET 6 1 DONE", &subject_line);
	memset(&subject_table, 0, sizeof(TABLE));
	subject_table.tb_First = &subject_line;
	ball.it_SubjectTable = &subject_table;

	CompileOK("get ANY ANY LET 5 1", &actor_line);
	SetActorTable(&player, &actor_line);

	WipeFlags();
	WordPtr = cmd;
	UserAction(&player, GetVerb());
	TEST_CHECK_EQ_INT(GetFlag(6), 1);	/* subject table ran */
	TEST_CHECK_EQ_INT(GetFlag(5), 0);	/* actor table did not */
}

static void test_subjecttable_without_done_falls_through(void)
{
	ITEM room, player, ball;
	LINE subject_line, actor_line;
	TABLE subject_table;
	char cmd[] = "get red ball";

	AddWord("get", 5, WD_VERB);
	AddWord("ball", 8, WD_NOUN);
	AddWord("red", 3, WD_ADJ);

	reset_item(&room); reset_item(&player); reset_item(&ball);
	ball.it_Adjective = 3; ball.it_Noun = 8;
	link_item(&player, &room);
	link_item(&ball, &room);
	SetMe(&player);
	SetPersona(0);

	CompileOK("get ANY ANY LET 6 1", &subject_line);	/* no DONE */
	memset(&subject_table, 0, sizeof(TABLE));
	subject_table.tb_First = &subject_line;
	ball.it_SubjectTable = &subject_table;

	CompileOK("get ANY ANY LET 5 1", &actor_line);
	SetActorTable(&player, &actor_line);

	WipeFlags();
	WordPtr = cmd;
	UserAction(&player, GetVerb());
	TEST_CHECK_EQ_INT(GetFlag(6), 1);	/* subject table ran */
	TEST_CHECK_EQ_INT(GetFlag(5), 1);	/* and so did the actor table */
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_full_pipeline_get_red_ball);
	RUN_TEST(test_item_in_inventory_is_also_found);
	RUN_TEST(test_unregistered_verb_is_not_recognized);
	RUN_TEST(test_nonexistent_item_leaves_item1_null);
	RUN_TEST(test_subjecttable_done_stops_actortable_fallthrough);
	RUN_TEST(test_subjecttable_without_done_falls_through);
	return test_summary_and_exit();
}
