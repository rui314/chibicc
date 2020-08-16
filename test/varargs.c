#include "test.h"
#include <stdarg.h>

int sum1(int x, ...) {
  va_list ap;
  va_start(ap, x);

  for (;;) {
    int y = va_arg(ap, int);
    if (y == 0)
      return x;
    x += y;
  }
}

int sum2(int x, ...) {
  va_list ap;
  va_start(ap, x);

  for (;;) {
    double y = va_arg(ap, double);
    x += y;

    int z = va_arg(ap, int);
    if (z == 0)
      return x;
    x += z;
  }
}

int main() {
  ASSERT(6, sum1(1, 2, 3, 0));
  ASSERT(21, sum2(1, 2.0, 3, 4.0, 5, 6.0, 0));

  printf("OK\n");
  return 0;
}
