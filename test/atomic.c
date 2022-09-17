#include "test.h"
#include <stdatomic.h>
#include <pthread.h>

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

  pthread_t thr1;
  pthread_t thr2;
  pthread_t thr3;

  pthread_create(&thr1, NULL, add1, &x);
  pthread_create(&thr2, NULL, add2, &x);
  pthread_create(&thr3, NULL, add3, &x);

  for (int i = 0; i < 1000*1000; i++)
    x--;

  pthread_join(thr1, NULL);
  pthread_join(thr2, NULL);
  pthread_join(thr3, NULL);
  return x;
}

static void fetch_ops(void) {
  _Atomic int x = 0;

  ASSERT(0, atomic_fetch_add(&x, 17));
  ASSERT(17, atomic_fetch_add(&x, 10));
  ASSERT(27, atomic_fetch_add(&x, 3));
  ASSERT(30, atomic_fetch_sub(&x, 17));
  ASSERT(13, atomic_fetch_sub(&x, 13));

  ASSERT(0, atomic_fetch_or(&x, 0xf0));
  ASSERT(0xf0, atomic_fetch_or(&x, 0x0f));
  ASSERT(0xff, atomic_fetch_and(&x, 0x0f));
  ASSERT(0x0f, atomic_fetch_xor(&x, 0xff));
  ASSERT(0xf0, atomic_fetch_add(&x, 0));
}

int main() {
  ASSERT(6*1000*1000, add_millions());

  ASSERT(3, ({ int x=3; atomic_exchange(&x, 5); }));
  ASSERT(5, ({ int x=3; atomic_exchange(&x, 5); x; }));
  
  fetch_ops();

  printf("OK\n");
  return 0;
}
