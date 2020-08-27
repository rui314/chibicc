#include "test.h"

int main() {
  ASSERT(4, sizeof(struct {int x:1; }));
  ASSERT(8, sizeof(struct {long x:1; }));

  struct bit1 {
    short a;
    char b;
    int c : 2;
    int d : 3;
    int e : 3;
  };

  ASSERT(4, sizeof(struct bit1));
  ASSERT(1, ({ struct bit1 x; x.a=1; x.b=2; x.c=3; x.d=4; x.e=5; x.a; }));
  ASSERT(1, ({ struct bit1 x={1,2,3,4,5}; x.a; }));
  ASSERT(2, ({ struct bit1 x={1,2,3,4,5}; x.b; }));
  ASSERT(-1, ({ struct bit1 x={1,2,3,4,5}; x.c; }));
  ASSERT(-4, ({ struct bit1 x={1,2,3,4,5}; x.d; }));
  ASSERT(-3, ({ struct bit1 x={1,2,3,4,5}; x.e; }));

  printf("OK\n");
  return 0;
}
