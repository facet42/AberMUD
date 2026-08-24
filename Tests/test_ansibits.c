#include "../Source/System.h"
#include "test_framework.h"

static void test_stricmp_case_insensitive(void)
{
	TEST_CHECK_EQ_INT(stricmp("hello", "HELLO"), 0);
	TEST_CHECK_EQ_INT(stricmp("MiXeD", "mixed"), 0);
	TEST_CHECK_EQ_INT(stricmp("", ""), 0);
}

static void test_stricmp_ordering(void)
{
	TEST_CHECK(stricmp("abc", "abd") < 0);
	TEST_CHECK(stricmp("abd", "abc") > 0);
}

/* Every call site in this codebase (FindInList, FreeWord, BreakWord) only
 * checks stricmp(...)==0, never its sign for ordering. That's just as well:
 * when one string is a strict prefix of the other, stricmp returns 1 in
 * BOTH directions (never -1), regardless of which argument is longer --
 * e.g. stricmp("ab","abc") and stricmp("abc","ab") are both 1. Pinning it
 * here so nobody starts relying on it as a real sort comparator without
 * noticing. */
static void test_stricmp_prefix_quirk(void)
{
	TEST_CHECK_EQ_INT(stricmp("ab", "abc"), 1);
	TEST_CHECK_EQ_INT(stricmp("abc", "ab"), 1);
}

static void test_strtok2_basic_split(void)
{
	char buf[] = "hello world foo";
	char *a = strtok2(buf, " ", " ");
	char *b = strtok2(NULL, " ", " ");
	char *c = strtok2(NULL, " ", " ");
	char *d = strtok2(NULL, " ", " ");
	TEST_CHECK_EQ_STR(a, "hello");
	TEST_CHECK_EQ_STR(b, "world");
	TEST_CHECK_EQ_STR(c, "foo");
	TEST_CHECK_NULL(d);
}

static void test_strtok2_brace_group(void)
{
	/* CompileTable.c reads text fields with strtok2(buf,"\t {","}"). The
	 * '{' FUDGE at AnsiBits.c:70 only fires when the brace is the very
	 * first non-delimiter character: it consumes just that brace and
	 * stops skipping further delimiters, so the token becomes everything
	 * up to the matching '}' (spaces included) rather than stopping at
	 * the first space as a normal word split would. */
	char buf[] = "{quoted phrase} next_field";
	char *a = strtok2(buf, "\t {", "}");
	char *b = strtok2(NULL, " \t{", "}");
	TEST_CHECK_EQ_STR(a, "quoted phrase");
	TEST_CHECK_EQ_STR(b, "next_field");
}

static void test_strtok2_unbraced_reads_to_end(void)
{
	/* Without a brace, there is no '}' to stop at, so a single call
	 * consumes the rest of the line as one token instead of splitting
	 * on the internal spaces -- this is how a whole free-text field
	 * (e.g. a description line) is read in one GetText2() call. */
	char buf[] = "  plain rest of line";
	char *a = strtok2(buf, "\t {", "}");
	char *b = strtok2(NULL, " \t{", "}");
	TEST_CHECK_EQ_STR(a, "plain rest of line");
	TEST_CHECK_NULL(b);
}

static void test_strtok2_empty_input(void)
{
	char buf[] = "";
	char *a = strtok2(buf, " ", " ");
	TEST_CHECK_NULL(a);
}

static void test_strtok2_all_delims(void)
{
	char buf[] = "   ";
	char *a = strtok2(buf, " ", " ");
	TEST_CHECK_NULL(a);
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_stricmp_case_insensitive);
	RUN_TEST(test_stricmp_ordering);
	RUN_TEST(test_stricmp_prefix_quirk);
	RUN_TEST(test_strtok2_basic_split);
	RUN_TEST(test_strtok2_brace_group);
	RUN_TEST(test_strtok2_unbraced_reads_to_end);
	RUN_TEST(test_strtok2_empty_input);
	RUN_TEST(test_strtok2_all_delims);
	return test_summary_and_exit();
}
