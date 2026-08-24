/* Minimal single-header test harness for AberMUD unit tests. No external
 * dependencies, so it builds the same way under gcc (Makefile) and
 * cl.exe (Visual Studio) without vendoring a third-party framework. */
#ifndef ABER_TEST_FRAMEWORK_H
#define ABER_TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

static int aber_test_failures = 0;
static int aber_test_count = 0;

#define TEST_CHECK(cond) \
	do { \
		aber_test_count++; \
		if (!(cond)) { \
			aber_test_failures++; \
			printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
		} \
	} while (0)

#define TEST_CHECK_EQ_INT(actual, expected) \
	do { \
		int _a = (actual), _e = (expected); \
		aber_test_count++; \
		if (_a != _e) { \
			aber_test_failures++; \
			printf("FAIL %s:%d: %s == %s (got %d, want %d)\n", \
				__FILE__, __LINE__, #actual, #expected, _a, _e); \
		} \
	} while (0)

#define TEST_CHECK_EQ_STR(actual, expected) \
	do { \
		const char *_a = (actual), *_e = (expected); \
		aber_test_count++; \
		if ((_a == NULL) != (_e == NULL) || (_a && _e && strcmp(_a, _e) != 0)) { \
			aber_test_failures++; \
			printf("FAIL %s:%d: %s == %s (got %s, want %s)\n", \
				__FILE__, __LINE__, #actual, #expected, \
				_a ? _a : "(null)", _e ? _e : "(null)"); \
		} \
	} while (0)

#define TEST_CHECK_NULL(actual) \
	do { \
		aber_test_count++; \
		if ((actual) != NULL) { \
			aber_test_failures++; \
			printf("FAIL %s:%d: %s == NULL\n", __FILE__, __LINE__, #actual); \
		} \
	} while (0)

#define RUN_TEST(fn) \
	do { \
		printf("-- %s\n", #fn); \
		fn(); \
	} while (0)

static int test_summary_and_exit(void)
{
	printf("%d checks, %d failed\n", aber_test_count, aber_test_failures);
	return aber_test_failures ? 1 : 0;
}

#endif
