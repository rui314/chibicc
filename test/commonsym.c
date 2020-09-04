#include "test.h"

int x;
int x = 5;
int y = 7;
int y;
int common_ext1;
int common_ext2;
static int common_local;

int main() {
  ASSERT(5, x);
  ASSERT(7, y);
  ASSERT(0, common_ext1);
  ASSERT(3, common_ext2);

  printf("OK\n");
  return 0;
}
