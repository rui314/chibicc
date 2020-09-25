#include "test.h"
#include "stddef.h"

int main() {
  ASSERT(5, ({ struct { char a; int b; } __attribute__((packed)) x; sizeof(x); }));
  ASSERT(0, offsetof(struct __attribute__((packed)) { char a; int b; }, a));
  ASSERT(1, offsetof(struct __attribute__((packed)) { char a; int b; }, b));

  ASSERT(5, ({ struct __attribute__((packed)) { char a; int b; } x; sizeof(x); }));
  ASSERT(0, offsetof(struct { char a; int b; } __attribute__((packed)), a));
  ASSERT(1, offsetof(struct { char a; int b; } __attribute__((packed)), b));

  ASSERT(9, ({ typedef struct { char a; int b[2]; } __attribute__((packed)) T; sizeof(T); }));
  ASSERT(9, ({ typedef struct __attribute__((packed)) { char a; int b[2]; } T; sizeof(T); }));

  ASSERT(1, offsetof(struct __attribute__((packed)) T { char a; int b[2]; }, b));
  ASSERT(1, _Alignof(struct __attribute__((packed)) { char a; int b[2]; }));

  printf("OK\n");
  return 0;
}
