CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

chibicc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): chibi.h

test: chibicc
	./chibicc tests > tmp.s
	gcc -static -o tmp tmp.s
	./tmp

clean:
	rm -f chibicc *.o *~ tmp*

.PHONY: test clean
