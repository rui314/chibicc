int assert(int expected, int actual, char *code);
int printf(char *fmt, ...);
int sprintf(char *buf, char *fmt, ...);
int strcmp(char *p, char *q);
int memcmp(char *p, char *q, long n);

#include "include1.h"

#

/* */ #

int main() {
  assert(5, include1, "include1");
  assert(7, include2, "include2");

  printf("OK\n");
  return 0;
}
