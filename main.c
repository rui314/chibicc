#include "chibicc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of arguments", argv[0]);

  // Tokenize and parse.
  user_input = argv[1];
  token = tokenize();
  Node *node = program();

  // Traverse the AST to emit assembly.
  codegen(node);
  return 0;
}
