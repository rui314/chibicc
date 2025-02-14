#include "test.h"

struct foo {
  int simple;
  int : 7;
  union {
    int anon;
    union tagged {
      float qux;
      long baz;
    } named;
  };
};

struct foo bar = { .simple = 1, .named.qux = 1.0 };

int main() {
  printf("OK\n");
  return 0;
}
