# Unit tests for the pure/parsing helpers identified in the coverage
# review (AnsiBits.c, Parser.c, FlagName.c). Each test binary links the
# real production .c file(s) under test straight out of Source/ -- not a
# copy -- plus engine_stubs.o to satisfy the handful of engine-wide
# symbols (SendItem, Error, Me, ...) that those files reference in code
# paths the tests never call.
#
# Run with: make -C Tests check

SRC     = ../Source
CC      = gcc -Wall -pedantic -I$(SRC)

BINS    = test_ansibits test_parser test_flagname

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

clean:
	rm -f $(BINS)

.PHONY: check clean
