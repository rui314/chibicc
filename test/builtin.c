#include "test.h"

int main() {
  ASSERT(1, __builtin_types_compatible_p(int, int));
  ASSERT(1, __builtin_types_compatible_p(double, double));
  ASSERT(0, __builtin_types_compatible_p(int, long));
  ASSERT(0, __builtin_types_compatible_p(long, float));
  ASSERT(1, __builtin_types_compatible_p(int *, int *));
  ASSERT(0, __builtin_types_compatible_p(short *, int *));
  ASSERT(0, __builtin_types_compatible_p(int **, int *));
  ASSERT(1, __builtin_types_compatible_p(const int, int));
  ASSERT(0, __builtin_types_compatible_p(unsigned, int));
  ASSERT(1, __builtin_types_compatible_p(signed, int));
  ASSERT(0, __builtin_types_compatible_p(struct {int a;}, struct {int a;}));

  ASSERT(1, __builtin_types_compatible_p(int (*)(void), int (*)(void)));
  ASSERT(1, __builtin_types_compatible_p(void (*)(int), void (*)(int)));
  ASSERT(1, __builtin_types_compatible_p(void (*)(int, double), void (*)(int, double)));
  ASSERT(1, __builtin_types_compatible_p(int (*)(float, double), int (*)(float, double)));
  ASSERT(0, __builtin_types_compatible_p(int (*)(float, double), int));
  ASSERT(0, __builtin_types_compatible_p(int (*)(float, double), int (*)(float)));
  ASSERT(0, __builtin_types_compatible_p(int (*)(float, double), int (*)(float, double, int)));
  ASSERT(1, __builtin_types_compatible_p(double (*)(...), double (*)(...)));
  ASSERT(0, __builtin_types_compatible_p(double (*)(...), double (*)(void)));

  ASSERT(1, ({ typedef struct {int a;} T; __builtin_types_compatible_p(T, T); }));
  ASSERT(1, ({ typedef struct {int a;} T; __builtin_types_compatible_p(T, const T); }));

  ASSERT(1, ({ struct {int a; int b;} x; __builtin_types_compatible_p(typeof(x.a), typeof(x.b)); }));

  printf("OK\n");
  return 0;
}
