#include "test.h"
#include <stdatomic.h>
#include <pthread.h>

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

  pthread_t thr1;
  pthread_t thr2;

  pthread_create(&thr1, NULL, add, &x);
  pthread_create(&thr2, NULL, add, &x);

  for (int i = 0; i < 1000*1000; i++)
    incr(&x);

  pthread_join(thr1, NULL);
  pthread_join(thr2, NULL);
  return x;
}

int main() {
  ASSERT(3*1000*1000, add_millions());

  ASSERT(3, ({ int x=3; atomic_exchange(&x, 5); }));
  ASSERT(5, ({ int x=3; atomic_exchange(&x, 5); x; }));

  printf("OK\n");
  return 0;
}
