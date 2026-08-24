#include "../Source/System.h"
#include "test_framework.h"

static WLIST *make_word(char *text, short type, short code, WLIST *next)
{
	WLIST *w = malloc(sizeof(WLIST));
	w->wd_Text = text;
	w->wd_Type = type;
	w->wd_Code = code;
	w->wd_Next = next;
	return w;
}

/* ---- FindInList ---- */

static void test_findinlist_empty_list(void)
{
	TEST_CHECK_NULL(FindInList(NULL, "sword", WD_NOUN));
}

static void test_findinlist_finds_matching_type(void)
{
	WLIST *list = make_word("sword", WD_NOUN, 1, NULL);
	WLIST *found = FindInList(list, "sword", WD_NOUN);
	TEST_CHECK(found == list);
}

static void test_findinlist_case_insensitive(void)
{
	WLIST *list = make_word("Sword", WD_NOUN, 1, NULL);
	TEST_CHECK(FindInList(list, "SWORD", WD_NOUN) == list);
}

static void test_findinlist_type_mismatch(void)
{
	WLIST *list = make_word("sword", WD_NOUN, 1, NULL);
	TEST_CHECK_NULL(FindInList(list, "sword", WD_VERB));
}

static void test_findinlist_type_zero_matches_any(void)
{
	WLIST *list = make_word("sword", WD_VERB, 1, NULL);
	TEST_CHECK(FindInList(list, "sword", 0) == list);
}

static void test_findinlist_walks_chain(void)
{
	WLIST *tail = make_word("axe", WD_NOUN, 2, NULL);
	WLIST *list = make_word("sword", WD_NOUN, 1, tail);
	TEST_CHECK(FindInList(list, "axe", WD_NOUN) == tail);
	TEST_CHECK_NULL(FindInList(list, "shield", WD_NOUN));
}

/* ---- BreakWord ---- */

static void test_breakword_leading_punctuation_ends_phrase(void)
{
	char fbuf[128];
	TEST_CHECK_NULL(BreakWord(".", fbuf, NULL, WD_NOISE));
	TEST_CHECK_NULL(BreakWord(";", fbuf, NULL, WD_NOISE));
	TEST_CHECK_NULL(BreakWord(",", fbuf, NULL, WD_NOISE));
}

static void test_breakword_single_char_tokens(void)
{
	char fbuf[128];
	char *rest = BreakWord(":rest", fbuf, NULL, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, ":");
	TEST_CHECK_EQ_STR(rest, "rest");
}

static void test_breakword_extracts_word_and_advances(void)
{
	char fbuf[128];
	char *rest = BreakWord("  take sword", fbuf, NULL, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "take");
	TEST_CHECK_EQ_STR(rest, " sword");
}

static void test_breakword_stops_at_terminator(void)
{
	char fbuf[128];
	char *rest = BreakWord("take,sword", fbuf, NULL, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "take");
	TEST_CHECK_EQ_STR(rest, ",sword");
}

static void test_breakword_end_of_input(void)
{
	char fbuf[128];
	TEST_CHECK_NULL(BreakWord("", fbuf, NULL, WD_NOISE));
	TEST_CHECK_NULL(BreakWord("   ", fbuf, NULL, WD_NOISE));
}

/* Regression tests for a fixed bug: BreakWord's `fbuf` parameter used to
 * be advanced in place while the word was copied ("*fbuf++=*iptr++"), so
 * by the time it reached the stricmp(fbuf,"AND"/"THEN") and
 * FindInList(skiplist,fbuf,...) checks, `fbuf` no longer pointed at the
 * start of the extracted word -- it pointed at the trailing NUL just
 * written, so those checks always compared "" against something and
 * could never match. BreakWord now keeps a separate `wbuf` pointer to the
 * buffer's start for those checks (mirroring how FNxPhrs already did it
 * correctly), and resets the write cursor back to it before copying each
 * candidate word so a skipped noise word doesn't leave its text in front
 * of the word that replaces it. */
static void test_breakword_detects_and_then(void)
{
	char fbuf[128];
	TEST_CHECK_NULL(BreakWord("and rest", fbuf, NULL, WD_NOISE));
	TEST_CHECK_NULL(BreakWord("THEN rest", fbuf, NULL, WD_NOISE));
}

static void test_breakword_skips_noise_words(void)
{
	char fbuf[128];
	WLIST *noise = make_word("the", WD_NOISE, 0, NULL);
	char *rest = BreakWord("the sword", fbuf, noise, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "sword");
	TEST_CHECK_EQ_STR(rest, "");
}

static void test_breakword_skips_multiple_noise_words(void)
{
	char fbuf[128];
	WLIST *a = make_word("a", WD_NOISE, 0, NULL);
	WLIST *the = make_word("the", WD_NOISE, 0, a);
	/* Each retry resets the write cursor to the buffer's start, so
	 * consecutive skipped noise words don't accumulate leftover text
	 * in front of the word that's finally kept. */
	char *rest = BreakWord("the a sword", fbuf, the, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "sword");
	TEST_CHECK_EQ_STR(rest, "");
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_findinlist_empty_list);
	RUN_TEST(test_findinlist_finds_matching_type);
	RUN_TEST(test_findinlist_case_insensitive);
	RUN_TEST(test_findinlist_type_mismatch);
	RUN_TEST(test_findinlist_type_zero_matches_any);
	RUN_TEST(test_findinlist_walks_chain);
	RUN_TEST(test_breakword_leading_punctuation_ends_phrase);
	RUN_TEST(test_breakword_single_char_tokens);
	RUN_TEST(test_breakword_extracts_word_and_advances);
	RUN_TEST(test_breakword_stops_at_terminator);
	RUN_TEST(test_breakword_end_of_input);
	RUN_TEST(test_breakword_detects_and_then);
	RUN_TEST(test_breakword_skips_noise_words);
	RUN_TEST(test_breakword_skips_multiple_noise_words);
	return test_summary_and_exit();
}
