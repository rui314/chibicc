CFLAGS=-std=c11 -g -fno-common

chibicc: main.o
	$(CC) -o chibicc main.o $(LDFLAGS)

test: chibicc
	./test.sh

clean:
	rm -f chibicc *.o *~ tmp*

.PHONY: test clean
