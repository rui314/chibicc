CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

chibicc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): chibi.h

chibicc-gen2: chibicc $(SRCS) chibi.h
	./self.sh

extern.o: tests-extern
	gcc -xc -c -o extern.o tests-extern

test: chibicc extern.o
	./chibicc tests > tmp.s
	gcc -static -o tmp tmp.s extern.o
	./tmp

test-gen2: chibicc-gen2 extern.o
	./chibicc-gen2 tests > tmp.s
	gcc -static -o tmp tmp.s extern.o
	./tmp

clean:
	rm -rf chibicc chibicc-gen* *.o *~ tmp*

.PHONY: test clean
