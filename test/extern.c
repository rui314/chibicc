#include "test.h"

extern int ext1;
extern int *ext2;

int main() {
  ASSERT(5, ext1);
  ASSERT(5, *ext2);

  printf("OK\n");
  return 0;
}
