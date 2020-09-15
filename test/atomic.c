#include "test.h"
#include <stdatomic.h>
#include <threads.h>

static int incr(int *p) {
  int oldval = *p;
  int newval;
  do {
    newval = oldval + 1;
  } while (!atomic_compare_exchange_weak(p, &oldval, newval));
  return newval;
}

static int add(void *arg) {
  int *x = arg;
  for (int i = 0; i < 1000*1000; i++)
    incr(x);
  return 0;
}

static int add_millions(void) {
  int x = 0;

  thrd_t thr1;
  thrd_t thr2;

  thrd_create(&thr1, add, &x);
  thrd_create(&thr2, add, &x);

  for (int i = 0; i < 1000*1000; i++)
    incr(&x);

  int res;
  thrd_join(thr1, &res);
  thrd_join(thr2, &res);
  return x;
}

int main() {
  ASSERT(3*1000*1000, add_millions());

  ASSERT(3, ({ int x=3; atomic_exchange(&x, 5); }));
  ASSERT(5, ({ int x=3; atomic_exchange(&x, 5); x; }));

  printf("OK\n");
  return 0;
}
