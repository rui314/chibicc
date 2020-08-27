#include "test.h"

char *asm_fn1(void) {
  asm("mov $50, %rax\n\t"
      "mov %rbp, %rsp\n\t"
      "pop %rbp\n\t"
      "ret");
}

char *asm_fn2(void) {
  asm inline volatile("mov $55, %rax\n\t"
                      "mov %rbp, %rsp\n\t"
                      "pop %rbp\n\t"
                      "ret");
}

int main() {
  ASSERT(50, asm_fn1());
  ASSERT(55, asm_fn2());

  printf("OK\n");
  return 0;
}
