#include "test.h"
#include <stdio.h>
#include <threads.h>

thread_local int v1;
thread_local int v2 = 5;
int v3 = 7;

int thread_main(void *unused) {
  ASSERT(0, v1);
  ASSERT(5, v2);
  ASSERT(7, v3);

  v1 = 1;
  v2 = 2;
  v3 = 3;

  ASSERT(1, v1);
  ASSERT(2, v2);
  ASSERT(3, v3);

  return 0;
}

int main() {
  thrd_t thr;

  ASSERT(0, v1);
  ASSERT(5, v2);
  ASSERT(7, v3);

  ASSERT(thrd_success, thrd_create(&thr, thread_main, NULL));
  ASSERT(thrd_success, thrd_join(thr, NULL));

  ASSERT(0, v1);
  ASSERT(5, v2);
  ASSERT(3, v3);

  printf("OK\n");
  return 0;
}
