#include "test.h"

int main() {
  ASSERT(1, sizeof(char));
  ASSERT(2, sizeof(short));
  ASSERT(2, sizeof(short int));
  ASSERT(2, sizeof(int short));
  ASSERT(4, sizeof(int));
  ASSERT(8, sizeof(long));
  ASSERT(8, sizeof(long int));
  ASSERT(8, sizeof(long int));
  ASSERT(8, sizeof(char *));
  ASSERT(8, sizeof(int *));
  ASSERT(8, sizeof(long *));
  ASSERT(8, sizeof(int **));
  ASSERT(8, sizeof(int(*)[4]));
  ASSERT(32, sizeof(int*[4]));
  ASSERT(16, sizeof(int[4]));
  ASSERT(48, sizeof(int[3][4]));
  ASSERT(8, sizeof(struct {int a; int b;}));

  ASSERT(8, sizeof(-10 + (long)5));
  ASSERT(8, sizeof(-10 - (long)5));
  ASSERT(8, sizeof(-10 * (long)5));
  ASSERT(8, sizeof(-10 / (long)5));
  ASSERT(8, sizeof((long)-10 + 5));
  ASSERT(8, sizeof((long)-10 - 5));
  ASSERT(8, sizeof((long)-10 * 5));
  ASSERT(8, sizeof((long)-10 / 5));

  ASSERT(1, ({ char i; sizeof(++i); }));
  ASSERT(1, ({ char i; sizeof(i++); }));

  ASSERT(8, sizeof(int(*)[10]));
  ASSERT(8, sizeof(int(*)[][10]));

  ASSERT(4, sizeof(struct { int x, y[]; }));

  ASSERT(1, sizeof(char));
  ASSERT(1, sizeof(signed char));
  ASSERT(1, sizeof(signed char signed));

  ASSERT(2, sizeof(short));
  ASSERT(2, sizeof(int short));
  ASSERT(2, sizeof(short int));
  ASSERT(2, sizeof(signed short));
  ASSERT(2, sizeof(int short signed));

  ASSERT(4, sizeof(int));
  ASSERT(4, sizeof(signed int));
  ASSERT(4, sizeof(signed));
  ASSERT(4, sizeof(signed signed));

  ASSERT(8, sizeof(long));
  ASSERT(8, sizeof(signed long));
  ASSERT(8, sizeof(signed long int));

  ASSERT(8, sizeof(long long));
  ASSERT(8, sizeof(signed long long));
  ASSERT(8, sizeof(signed long long int));

  printf("OK\n");
  return 0;
}
