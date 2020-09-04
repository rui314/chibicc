#include "test.h"

int main() {
  ASSERT(20, ({ int n=5; int x[n]; sizeof(x); }));
  ASSERT((5+1)*(8*2)*4, ({ int m=5, n=8; int x[m+1][n*2]; sizeof(x); }));

  ASSERT(8, ({ char n=10; int (*x)[n][n+2]; sizeof(x); }));
  ASSERT(480, ({ char n=10; int (*x)[n][n+2]; sizeof(*x); }));
  ASSERT(48, ({ char n=10; int (*x)[n][n+2]; sizeof(**x); }));
  ASSERT(4, ({ char n=10; int (*x)[n][n+2]; sizeof(***x); }));

  ASSERT(60, ({ char n=3; int x[5][n]; sizeof(x); }));
  ASSERT(12, ({ char n=3; int x[5][n]; sizeof(*x); }));

  ASSERT(60, ({ char n=3; int x[n][5]; sizeof(x); }));
  ASSERT(20, ({ char n=3; int x[n][5]; sizeof(*x); }));

  printf("OK\n");
  return 0;
}
