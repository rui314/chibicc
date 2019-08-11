#include "test.h"

int main() {
  ASSERT(97, 'a');
  ASSERT(10, '\n');
  ASSERT(-128, '\x80');

  printf("OK\n");
  return 0;
}
