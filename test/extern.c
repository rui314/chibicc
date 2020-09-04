#include "test.h"

extern int ext1;
extern int *ext2;

int main() {
  ASSERT(5, ext1);
  ASSERT(5, *ext2);

  extern int ext3;
  assert(7, ext3);

  int ext_fn1(int x);
  assert(5, ext_fn1(5));

  extern int ext_fn2(int x);
  assert(8, ext_fn2(8));

  printf("OK\n");
  return 0;
}
