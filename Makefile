CC=gcc
CFLAGS=-std=c11 -g -fno-common -Wall -Wno-switch

OBJECT=chibicc
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.exe)

# Stage 1

$(OBJECT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): $(OBJECT).h

test/%.exe: $(OBJECT) test/%.c
	./$(OBJECT) -Iinclude -Itest -c -o test/$*.o test/$*.c
	$(CC) -pthread -o $@ test/$*.o -xc test/common

test: $(TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done
	test/driver.sh ./$(OBJECT)

test-all: test test-stage2

# Stage 2

stage2/$(OBJECT): $(OBJS:%=stage2/%)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stage2/%.o: $(OBJECT) %.c
	mkdir -p stage2/test
	./mycc -c -o $(@D)/$*.o $*.c

stage2/test/%.exe: stage2/$(OBJECT) test/%.c
	mkdir -p stage2/test
	./stage2/$(OBJECT) -Iinclude -Itest -c -o stage2/test/$*.o test/$*.c
	$(CC) -pthread -o $@ stage2/test/$*.o -xc test/common

test-stage2: $(TESTS:test/%=stage2/test/%)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done
	test/driver.sh ./stage2/$(OBJECT)

# Misc.

clean:
	rm -rf $(OBJECT) tmp* $(TESTS) test/*.s test/*.exe stage2
	find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'

.PHONY: test clean test-stage2
