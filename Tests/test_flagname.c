#include "../Source/System.h"
#include "test_framework.h"

/* FindRBit/FindPBit/FindOBit/FindCBit are the only public entry points to
 * the static WhichBitField() linear search; exercise it through them
 * against the real RBitNames/PBitNames/OBitNames/CBitNames arrays
 * (extern'd in prototype.h, defined in FlagName.c). */

static void reset_bit_names(void)
{
	int i;
	for (i = 0; i < 16; i++) {
		RBitNames[i] = NULL;
		PBitNames[i] = NULL;
		OBitNames[i] = NULL;
		CBitNames[i] = NULL;
	}
}

static void test_find_bit_not_found_on_empty(void)
{
	reset_bit_names();
	TEST_CHECK_EQ_INT(FindRBit("dark"), -1);
	TEST_CHECK_EQ_INT(FindPBit("blind"), -1);
	TEST_CHECK_EQ_INT(FindOBit("locked"), -1);
	TEST_CHECK_EQ_INT(FindCBit("wizard"), -1);
}

static void test_find_bit_matches_case_insensitively(void)
{
	reset_bit_names();
	RBitNames[3] = "DARK";
	TEST_CHECK_EQ_INT(FindRBit("dark"), 3);
	TEST_CHECK_EQ_INT(FindRBit("DaRk"), 3);
}

static void test_find_bit_returns_first_slot_scanned(void)
{
	reset_bit_names();
	OBitNames[0] = "locked";
	OBitNames[15] = "locked";
	TEST_CHECK_EQ_INT(FindOBit("locked"), 0);
}

static void test_find_bit_distinguishes_arrays(void)
{
	reset_bit_names();
	PBitNames[5] = "blind";
	TEST_CHECK_EQ_INT(FindPBit("blind"), 5);
	TEST_CHECK_EQ_INT(FindRBit("blind"), -1);
	TEST_CHECK_EQ_INT(FindOBit("blind"), -1);
	TEST_CHECK_EQ_INT(FindCBit("blind"), -1);
}

static void test_find_bit_no_match(void)
{
	reset_bit_names();
	CBitNames[2] = "wizard";
	TEST_CHECK_EQ_INT(FindCBit("mortal"), -1);
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_find_bit_not_found_on_empty);
	RUN_TEST(test_find_bit_matches_case_insensitively);
	RUN_TEST(test_find_bit_returns_first_slot_scanned);
	RUN_TEST(test_find_bit_distinguishes_arrays);
	RUN_TEST(test_find_bit_no_match);
	return test_summary_and_exit();
}
