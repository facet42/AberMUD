/*
 * Tests for the table compiler (CompileTable.c): compiles human-
 * readable table source text through the real EncodeEntry()/
 * EncodeLine()/EncodeCommand()/EncodeWord()/EncodeNumber()/EncodeFlag()/
 * EncodeItem() chain, and where useful executes the result through the
 * same ExecTable()/RunLine() path test_vm.c uses, so a rule is checked
 * end to end: text in, game-state change out. Also exercises
 * Decompress(), the inverse pretty-printer used by in-game table
 * editing/listing commands.
 *
 * The literal-item-reference test in particular closes the loop on the
 * pointer-packing fix: it compiles a real (non-$-sentinel) item
 * reference through the real EncodeItem()/WriteDbPtr() encoder -- not
 * the hand-rolled encode_ptr() helper test_vm.c uses -- and confirms
 * ArgItem() decodes back the exact same pointer at execution time.
 */
#include "../Source/System.h"
#include "../Source/User.h"
#include "test_framework.h"

extern int Noun1, Adj1, Noun2, Adj2, Verb, Prep;
extern ITEM CompileTestFakeMasterItem;

static void reset_item(ITEM *i)
{
	memset(i, 0, sizeof(ITEM));
	i->it_Adjective = -1;
	i->it_Noun = -1;
}

/* Compiles `src` into `line` against a fresh dummy actor, returning
 * EncodeEntry()'s status. */
static int Compile(char *src, LINE *line)
{
	ITEM dummy_actor;
	reset_item(&dummy_actor);
	memset(line, 0, sizeof(LINE));
	LoadLineBuffer(src);
	return EncodeEntry(&dummy_actor, line);
}

/* Runs a compiled line the way a real command dispatch would: sets the
 * parsed-word globals RunLine's ArgMatch() checks against, then
 * executes. */
static int RunCompiled(LINE *line, int verb, int noun1, int noun2)
{
	TABLE table;
	Verb = verb; Noun1 = noun1; Noun2 = noun2;
	memset(&table, 0, sizeof(TABLE));
	table.tb_First = line;
	return ExecTable(&table);
}

static void test_compiles_verb_and_nouns(void)
{
	LINE line;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);

	TEST_CHECK_EQ_INT(Compile("get thing thing AT $1 LET 5 42", &line), 0);
	TEST_CHECK_EQ_INT(line.li_Verb, 5);
	TEST_CHECK_EQ_INT(line.li_Noun1, 7);
	TEST_CHECK_EQ_INT(line.li_Noun2, 7);
}

static void test_any_and_none_compile_to_wordmatch_sentinels(void)
{
	LINE line;
	/* ArgMatch()'s WORDMATCH macro (TableDriver.c): -1 means "any",
	 * -2 means "none" (only matches an absent/-1 word). */
	TEST_CHECK_EQ_INT(Compile("ANY ANY NONE LET 5 1", &line), 0);
	TEST_CHECK_EQ_INT(line.li_Verb, -1);
	TEST_CHECK_EQ_INT(line.li_Noun1, -1);
	TEST_CHECK_EQ_INT(line.li_Noun2, -2);
}

static void test_execution_gated_by_verb_noun_match(void)
{
	LINE line;
	ITEM room, player;
	extern ITEM *Item1;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);
	AddWord("drop", 6, WD_VERB);
	reset_item(&room); reset_item(&player);
	player.it_Parent = &room;
	SetMe(&player);
	Item1 = &room;

	TEST_CHECK_EQ_INT(Compile("get thing thing AT $1 LET 5 42", &line), 0);

	WipeFlags();
	RunCompiled(&line, 6, 7, 7);	/* wrong verb: "drop" instead of "get" */
	TEST_CHECK_EQ_INT(GetFlag(5), 0);

	RunCompiled(&line, 5, 7, 7);	/* right verb/nouns: "get thing thing" */
	TEST_CHECK_EQ_INT(GetFlag(5), 42);
}

static void test_any_verb_matches_anything(void)
{
	LINE line;
	ITEM room, player;
	extern ITEM *Item1;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);
	reset_item(&room); reset_item(&player);
	player.it_Parent = &room;
	SetMe(&player);
	Item1 = &room;

	TEST_CHECK_EQ_INT(Compile("ANY thing thing AT $1 LET 5 7", &line), 0);

	WipeFlags();
	RunCompiled(&line, 5, 7, 7);
	TEST_CHECK_EQ_INT(GetFlag(5), 7);
}

static void test_encode_number_literal_and_flag_indirection(void)
{
	LINE line;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);

	WipeFlags();
	SetFlag(10, 123);
	/* "F10" as LET's value argument means "read flag 10", not the
	 * literal number 10 -- ArgNum()'s >=30000 flag-indirection
	 * convention (TableDriver.c), which EncodeNumber's 'F'/'f' prefix
	 * compiles to (n + 30000). */
	TEST_CHECK_EQ_INT(Compile("get thing thing LET 5 F10", &line), 0);
	RunCompiled(&line, 5, 7, 7);
	TEST_CHECK_EQ_INT(GetFlag(5), 123);
}

static void test_encode_item_sentinel_roundtrip(void)
{
	LINE line;
	ITEM room, elsewhere, player;
	extern ITEM *Item1;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);
	reset_item(&room); reset_item(&elsewhere); reset_item(&player);
	player.it_Parent = &room;
	SetMe(&player);

	TEST_CHECK_EQ_INT(Compile("get thing thing AT $1 LET 5 1", &line), 0);

	WipeFlags();
	Item1 = &room;
	RunCompiled(&line, 5, 7, 7);
	TEST_CHECK_EQ_INT(GetFlag(5), 1);

	WipeFlags();
	Item1 = &elsewhere;
	RunCompiled(&line, 5, 7, 7);
	TEST_CHECK_EQ_INT(GetFlag(5), 0);
}

/* The pointer-packing integration test: compiles a literal (non-$-
 * sentinel) item reference through the real EncodeItem()/WriteDbPtr(),
 * and confirms the real ArgItem()/PairArg() decode it back to the
 * exact same pointer at execution time. FindMaster() is stubbed
 * (compile_stubs.c) to always resolve to CompileTestFakeMasterItem,
 * simulating "there is exactly one player-visible item matching this
 * adjective/noun in the game". */
static void test_encode_item_literal_reference(void)
{
	LINE line;
	ITEM player;
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);
	AddWord("red", 2, WD_ADJ);
	reset_item(&player);
	reset_item(&CompileTestFakeMasterItem);
	CompileTestFakeMasterItem.it_Adjective = 2;
	CompileTestFakeMasterItem.it_Noun = 7;
	player.it_Parent = &CompileTestFakeMasterItem;
	SetMe(&player);

	TEST_CHECK_EQ_INT(Compile("get thing thing AT red thing LET 5 1", &line), 0);

	WipeFlags();
	RunCompiled(&line, 5, 7, 7);
	TEST_CHECK_EQ_INT(GetFlag(5), 1);
}

static void test_encode_unknown_verb_fails(void)
{
	LINE line;
	TEST_CHECK_EQ_INT(Compile("nosuchverb ANY ANY LET 5 1", &line), -1);
}

static void test_encode_unknown_condition_fails(void)
{
	LINE line;
	AddWord("get", 5, WD_VERB);
	TEST_CHECK_EQ_INT(Compile("get ANY ANY NOSUCHCONDITION 5 1", &line), -1);
}

static void test_decompress_roundtrip_contains_source_tokens(void)
{
	LINE line;
	char buf[512];
	AddWord("get", 5, WD_VERB);
	AddWord("thing", 7, WD_NOUN);

	TEST_CHECK_EQ_INT(Compile("get thing thing AT $1", &line), 0);
	buf[0] = 0;
	Decompress(&line, buf);
	TEST_CHECK(strstr(buf, "get") != NULL);
	TEST_CHECK(strstr(buf, "thing") != NULL);
	TEST_CHECK(strstr(buf, "AT") != NULL);
	TEST_CHECK(strstr(buf, "$1") != NULL);
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_compiles_verb_and_nouns);
	RUN_TEST(test_any_and_none_compile_to_wordmatch_sentinels);
	RUN_TEST(test_execution_gated_by_verb_noun_match);
	RUN_TEST(test_any_verb_matches_anything);
	RUN_TEST(test_encode_number_literal_and_flag_indirection);
	RUN_TEST(test_encode_item_sentinel_roundtrip);
	RUN_TEST(test_encode_item_literal_reference);
	RUN_TEST(test_encode_unknown_verb_fails);
	RUN_TEST(test_encode_unknown_condition_fails);
	RUN_TEST(test_decompress_roundtrip_contains_source_tokens);
	return test_summary_and_exit();
}
