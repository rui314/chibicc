#include "test.h"

int main() {
  ASSERT(0, 0);
  ASSERT(42, 42);
  ASSERT(21, 5+20-4);
  ASSERT(41,  12 + 34 - 5 );
  ASSERT(47, 5+6*7);
  ASSERT(15, 5*(9-6));
  ASSERT(4, (3+5)/2);
  ASSERT(10, -10+20);
  ASSERT(10, - -10);
  ASSERT(10, - - +10);

  ASSERT(0, 0==1);
  ASSERT(1, 42==42);
  ASSERT(1, 0!=1);
  ASSERT(0, 42!=42);

  ASSERT(1, 0<1);
  ASSERT(0, 1<1);
  ASSERT(0, 2<1);
  ASSERT(1, 0<=1);
  ASSERT(1, 1<=1);
  ASSERT(0, 2<=1);

  ASSERT(1, 1>0);
  ASSERT(0, 1>1);
  ASSERT(0, 1>2);
  ASSERT(1, 1>=0);
  ASSERT(1, 1>=1);
  ASSERT(0, 1>=2);

  ASSERT(0, 1073741824 * 100 / 100);

  ASSERT(7, ({ int i=2; i+=5; i; }));
  ASSERT(7, ({ int i=2; i+=5; }));
  ASSERT(3, ({ int i=5; i-=2; i; }));
  ASSERT(3, ({ int i=5; i-=2; }));
  ASSERT(6, ({ int i=3; i*=2; i; }));
  ASSERT(6, ({ int i=3; i*=2; }));
  ASSERT(3, ({ int i=6; i/=2; i; }));
  ASSERT(3, ({ int i=6; i/=2; }));

  ASSERT(3, ({ int i=2; ++i; }));
  ASSERT(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; ++*p; }));
  ASSERT(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; --*p; }));

  printf("OK\n");
  return 0;
}
