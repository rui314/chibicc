#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
    return 1;
  }

  printf("  .globl main\n");
  printf("main:\n");
  printf("  mov $%d, %%rax\n", atoi(argv[1]));
  printf("  ret\n");
  return 0;
}
