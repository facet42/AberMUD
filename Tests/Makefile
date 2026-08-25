# Unit tests for the pure/parsing helpers (AnsiBits.c, Parser.c,
# FlagName.c) and the action/condition VM (CondCode.c, ActionCode.c,
# TableDriver.c) identified in the coverage review. Each test binary
# links the real production .c file(s) under test straight out of
# Source/ -- not a copy -- plus a stub file to satisfy the engine-wide
# symbols those files reference in code paths the tests never call.
#
# Run with: make -C Tests check

SRC     = ../Source
CC      = gcc -Wall -pedantic -I$(SRC)

BINS    = test_ansibits test_parser test_flagname test_vm test_saveload test_compile

check: $(BINS)
	@status=0; \
	for t in $(BINS); do \
		echo "=== $$t ==="; \
		./$$t || status=1; \
	done; \
	exit $$status

test_ansibits: test_ansibits.c $(SRC)/AnsiBits.c test_framework.h
	$(CC) test_ansibits.c $(SRC)/AnsiBits.c -o $@

test_parser: test_parser.c $(SRC)/Parser.c engine_stubs.c test_framework.h
	$(CC) test_parser.c $(SRC)/Parser.c $(SRC)/AnsiBits.c engine_stubs.c -o $@

test_flagname: test_flagname.c $(SRC)/FlagName.c $(SRC)/Parser.c engine_stubs.c test_framework.h
	$(CC) test_flagname.c $(SRC)/FlagName.c $(SRC)/Parser.c $(SRC)/AnsiBits.c engine_stubs.c -o $@

test_vm: test_vm.c $(SRC)/CondCode.c $(SRC)/ActionCode.c $(SRC)/TableDriver.c vm_stubs.c test_framework.h
	$(CC) test_vm.c $(SRC)/CondCode.c $(SRC)/ActionCode.c $(SRC)/TableDriver.c $(SRC)/AnsiBits.c vm_stubs.c -o $@

# test_saveload.c #includes SaveLoad.c's source directly (to reach its
# static functions), so SaveLoad.c is NOT also passed here -- that
# would define everything twice.
test_saveload: test_saveload.c $(SRC)/SaveLoad.c saveload_stubs.c test_framework.h
	$(CC) test_saveload.c saveload_stubs.c -o $@

test_compile: test_compile.c $(SRC)/CompileTable.c $(SRC)/Parser.c $(SRC)/CondCode.c $(SRC)/ActionCode.c $(SRC)/TableDriver.c $(SRC)/FlagName.c compile_stubs.c test_framework.h
	$(CC) test_compile.c $(SRC)/CompileTable.c $(SRC)/Parser.c $(SRC)/CondCode.c $(SRC)/ActionCode.c $(SRC)/TableDriver.c $(SRC)/FlagName.c $(SRC)/AnsiBits.c compile_stubs.c -o $@

clean:
	rm -f $(BINS)

.PHONY: check clean
