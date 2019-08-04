#include "chibi.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of arguments", argv[0]);

  // Tokenize and parse.
  user_input = argv[1];
  token = tokenize();
  Function *prog = program();

  // Assign offsets to local variables.
  for (Function *fn = prog; fn; fn = fn->next) {
    int offset = 0;
    for (VarList *vl = fn->locals; vl; vl = vl->next) {
      offset += 8;
      vl->var->offset = offset;
    }
    fn->stack_size = offset;
  }

  // Traverse the AST to emit assembly.
  codegen(prog);

  return 0;
}
