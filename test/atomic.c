#include "test.h"
#include <stdatomic.h>
#include <threads.h>

static int incr(_Atomic int *p) {
  int oldval = *p;
  int newval;
  do {
    newval = oldval + 1;
  } while (!atomic_compare_exchange_weak(p, &oldval, newval));
  return newval;
}

static int add1(void *arg) {
  _Atomic int *x = arg;
  for (int i = 0; i < 1000*1000; i++)
    incr(x);
  return 0;
}

static int add2(void *arg) {
  _Atomic int *x = arg;
  for (int i = 0; i < 1000*1000; i++)
    (*x)++;
  return 0;
}

static int add3(void *arg) {
  _Atomic int *x = arg;
  for (int i = 0; i < 1000*1000; i++)
    *x += 5;
  return 0;
}

static int add_millions(void) {
  _Atomic int x = 0;

  thrd_t thr1;
  thrd_t thr2;
  thrd_t thr3;

  thrd_create(&thr1, add1, &x);
  thrd_create(&thr2, add2, &x);
  thrd_create(&thr3, add3, &x);

  for (int i = 0; i < 1000*1000; i++)
    x--;

  int res;
  thrd_join(thr1, &res);
  thrd_join(thr2, &res);
  thrd_join(thr3, &res);
  return x;
}

int main() {
  ASSERT(6*1000*1000, add_millions());

  ASSERT(3, ({ int x=3; atomic_exchange(&x, 5); }));
  ASSERT(5, ({ int x=3; atomic_exchange(&x, 5); x; }));

  printf("OK\n");
  return 0;
}
