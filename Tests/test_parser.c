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

/* BUG (pre-existing, also present in Original/Parser.c -- not introduced by
 * this fork): BreakWord's `fbuf` parameter is advanced in place while the
 * word is copied ("*fbuf++=*iptr++"), so by the time it reaches the
 * stricmp(fbuf,"AND"/"THEN") and FindInList(skiplist,fbuf,...) checks,
 * `fbuf` no longer points at the start of the extracted word -- it points
 * at the trailing NUL just written. Every one of those checks is
 * comparing "" against something, so it can never match. Contrast with
 * FNxPhrs() a little further down this file, which keeps a separate `fb`
 * pointer to the buffer's start for exactly this reason and does not have
 * the bug. Net effect: BreakWord's "AND"/"THEN" phrase-end detection and
 * its noise-word skipping are both dead code today. These tests pin the
 * current (buggy) behavior so it doesn't get silently changed further;
 * see the coverage report for whether to fix it. */
static void test_breakword_and_then_never_matched_due_to_bug(void)
{
	char fbuf[128];
	char *rest;
	rest = BreakWord("and rest", fbuf, NULL, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "and");
	TEST_CHECK_EQ_STR(rest, " rest");
	rest = BreakWord("THEN rest", fbuf, NULL, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "THEN");
	TEST_CHECK_EQ_STR(rest, " rest");
}

static void test_breakword_noise_words_never_skipped_due_to_bug(void)
{
	char fbuf[128];
	WLIST *noise = make_word("the", WD_NOISE, 0, NULL);
	char *rest = BreakWord("the sword", fbuf, noise, WD_NOISE);
	TEST_CHECK_EQ_STR(fbuf, "the");
	TEST_CHECK_EQ_STR(rest, " sword");
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
	RUN_TEST(test_breakword_and_then_never_matched_due_to_bug);
	RUN_TEST(test_breakword_noise_words_never_skipped_due_to_bug);
	return test_summary_and_exit();
}
