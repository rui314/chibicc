#include "test.h"

int ret3() {
  return 3;
  return 5;
}

int add2(int x, int y) {
  return x + y;
}

int sub2(int x, int y) {
  return x - y;
}

int add6(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}

int addx(int *x, int y) {
  return *x + y;
}

int sub_char(char a, char b, char c) {
  return a - b - c;
}

int fib(int x) {
  if (x<=1)
    return 1;
  return fib(x-1) + fib(x-2);
}

int sub_long(long a, long b, long c) {
  return a - b - c;
}

int sub_short(short a, short b, short c) {
  return a - b - c;
}

int g1;

int *g1_ptr() { return &g1; }
char int_to_char(int x) { return x; }

int div_long(long a, long b) {
  return a / b;
}

_Bool bool_fn_add(_Bool x) { return x + 1; }
_Bool bool_fn_sub(_Bool x) { return x - 1; }

int main() {
  ASSERT(3, ret3());
  ASSERT(8, add2(3, 5));
  ASSERT(2, sub2(5, 3));
  ASSERT(21, add6(1,2,3,4,5,6));
  ASSERT(66, add6(1,2,add6(3,4,5,6,7,8),9,10,11));
  ASSERT(136, add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16));

  ASSERT(7, add2(3,4));
  ASSERT(1, sub2(4,3));
  ASSERT(55, fib(9));

  ASSERT(1, ({ sub_char(7, 3, 3); }));

  ASSERT(1, sub_long(7, 3, 3));
  ASSERT(1, sub_short(7, 3, 3));

  g1 = 3;

  ASSERT(3, *g1_ptr());
  ASSERT(5, int_to_char(261));
  ASSERT(5, int_to_char(261));
  ASSERT(-5, div_long(-10, 2));

  ASSERT(1, bool_fn_add(3));
  ASSERT(0, bool_fn_sub(3));
  ASSERT(1, bool_fn_add(-3));
  ASSERT(0, bool_fn_sub(-3));
  ASSERT(1, bool_fn_add(0));
  ASSERT(1, bool_fn_sub(0));

  printf("OK\n");
  return 0;
}
