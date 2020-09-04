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

char *fmt(char *buf, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  return buf;
}

int main() {
  ASSERT(6, sum1(1, 2, 3, 0));
  ASSERT(55, sum1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0));
  ASSERT(21, sum2(1, 2.0, 3, 4.0, 5, 6.0, 0));
  ASSERT(21, sum2(1, 2.0, 3, 4.0, 5, 6.0, 0));
  ASSERT(210, sum2(1, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0, 11, 12.0, 13, 14.0, 15, 16.0, 17, 18.0, 19, 20.0, 0));

  printf("OK\n");
  return 0;
}
