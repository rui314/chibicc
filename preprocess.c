#include "chibicc.h"

// Entry point function of the preprocessor.
Token *preprocess(Token *tok) {
  convert_keywords(tok);
  return tok;
}
