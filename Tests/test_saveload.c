/*
 * Tests for SaveLoad.c's serialization primitives (SaveShort/LoadShort,
 * SaveLong/LoadLong, SaveString/LoadString, LoadItem's -1-means-NULL
 * sentinel).
 *
 * SaveShort/LoadShort/SaveLong/LoadLong/SaveString/LoadString/LoadItem
 * are all `static` in SaveLoad.c, so there is no way to link against
 * them from a separate translation unit. This file #includes
 * SaveLoad.c's source directly instead of linking its .o, which pulls
 * its static functions (and static globals like ItemArray, Load_Error)
 * into this same translation unit where the tests can call them
 * directly -- a standard technique for unit-testing static functions
 * without weakening their linkage just for tests. saveload_stubs.c
 * (linked normally) supplies the ~45 other engine functions SaveLoad.c
 * references in code paths these tests do not exercise.
 *
 * LoadLong's regression test below is the one that matters most: this
 * exact function shipped with a sign-extension bug (see the commit
 * that fixed it) that only surfaced when the real server tried to boot
 * a real world file on a 64-bit build -- SaveItem()'s -1 "no item"
 * sentinel came back as 4294967295 instead of -1, and LoadItem() used
 * it to index ItemArray with a garbage value. A test like this one
 * would have caught it without needing to boot the whole server.
 */
#include "../Source/SaveLoad.c"
#include "test_framework.h"

static void test_saveshort_loadshort_roundtrip(void)
{
	unsigned short values[] = { 0, 1, 255, 256, 32767, 32768, 65535 };
	size_t n;
	for (n = 0; n < sizeof(values) / sizeof(values[0]); n++) {
		FILE *f = tmpfile();
		unsigned short got;
		SaveShort(f, values[n]);
		rewind(f);
		got = LoadShort(f);
		TEST_CHECK_EQ_INT(got, values[n]);
		fclose(f);
	}
}

static void test_savelong_loadlong_roundtrip(void)
{
	/* -1 is the interesting one: SaveItem() uses it as its "no item"
	 * sentinel, and it is exactly what the sign-extension bug broke. */
	long values[] = { 0, 1, -1, 100000, -100000, 2147483647L, -2147483647L - 1 };
	size_t n;
	for (n = 0; n < sizeof(values) / sizeof(values[0]); n++) {
		FILE *f = tmpfile();
		long got;
		SaveLong(f, values[n]);
		rewind(f);
		got = LoadLong(f);
		TEST_CHECK_EQ_INT(got, values[n]);
		fclose(f);
	}
}

static void test_savestring_loadstring_roundtrip(void)
{
	FILE *f = tmpfile();
	TPTR original = AllocText("the quick brown fox");
	TPTR loaded;
	SaveString(f, original);
	rewind(f);
	loaded = LoadString(f);
	TEST_CHECK_EQ_STR(TextOf(loaded), "the quick brown fox");
	fclose(f);
}

static void test_savestring_loadstring_empty(void)
{
	FILE *f = tmpfile();
	TPTR original = AllocText("");
	TPTR loaded;
	SaveString(f, original);
	rewind(f);
	loaded = LoadString(f);
	TEST_CHECK_EQ_STR(TextOf(loaded), "");
	fclose(f);
}

/* The actual regression: LoadItem()'s "x < 0 means NULL" check against
 * SaveItem()'s -1 sentinel, and correct positive-index lookup into
 * ItemArray -- both exercised the way LoadObject() really uses them
 * for it_Next/it_Children/it_Parent/it_Superclass. */
static void test_loaditem_null_sentinel_and_lookup(void)
{
	ITEM real_items[3];
	ITEM *fake_array[3];
	ITEM **saved_array = ItemArray;
	int idx;

	for (idx = 0; idx < 3; idx++)
		fake_array[idx] = &real_items[idx];
	ItemArray = fake_array;

	{
		FILE *f = tmpfile();
		ITEM *got;
		SaveLong(f, -1L);
		rewind(f);
		got = LoadItem(f);
		TEST_CHECK_NULL(got);
		fclose(f);
	}
	{
		FILE *f = tmpfile();
		ITEM *got;
		SaveLong(f, 2L);
		rewind(f);
		got = LoadItem(f);
		TEST_CHECK(got == fake_array[2]);
		fclose(f);
	}

	ItemArray = saved_array;
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_saveshort_loadshort_roundtrip);
	RUN_TEST(test_savelong_loadlong_roundtrip);
	RUN_TEST(test_savestring_loadstring_roundtrip);
	RUN_TEST(test_savestring_loadstring_empty);
	RUN_TEST(test_loaditem_null_sentinel_and_lookup);
	return test_summary_and_exit();
}
