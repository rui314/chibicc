#include "chibicc.h"

static bool is_hash(Token *tok) {
  return tok->at_bol && equal(tok, "#");
}

// Some preprocessor directives such as #include allow extraneous
// tokens before newline. This function skips such tokens.
static Token *skip_line(Token *tok) {
  if (tok->at_bol)
    return tok;
  warn_tok(tok, "extra token");
  while (tok->at_bol)
    tok = tok->next;
  return tok;
}

static Token *copy_token(Token *tok) {
  Token *t = calloc(1, sizeof(Token));
  *t = *tok;
  t->next = NULL;
  return t;
}

// Append tok2 to the end of tok1.
static Token *append(Token *tok1, Token *tok2) {
  if (!tok1 || tok1->kind == TK_EOF)
    return tok2;

  Token head = {};
  Token *cur = &head;

  for (; tok1 && tok1->kind != TK_EOF; tok1 = tok1->next)
    cur = cur->next = copy_token(tok1);
  cur->next = tok2;
  return head.next;
}

// Visit all tokens in `tok` while evaluating preprocessing
// macros and directives.
static Token *preprocess2(Token *tok) {
  Token head = {};
  Token *cur = &head;

  while (tok->kind != TK_EOF) {
    // Pass through if it is not a "#".
    if (!is_hash(tok)) {
      cur = cur->next = tok;
      tok = tok->next;
      continue;
    }

    tok = tok->next;

    if (equal(tok, "include")) {
      tok = tok->next;

      if (tok->kind != TK_STR)
        error_tok(tok, "expected a filename");

      char *path;
      if (tok->str[0] == '/')
        path = tok->str;
      else
        path = format("%s/%s", dirname(strdup(tok->file->name)), tok->str);

      Token *tok2 = tokenize_file(path);
      if (!tok2)
        error_tok(tok, "%s", strerror(errno));
      tok = skip_line(tok->next);
      tok = append(tok2, tok);
      continue;
    }

    // `#`-only line is legal. It's called a null directive.
    if (tok->at_bol)
      continue;

    error_tok(tok, "invalid preprocessor directive");
  }

  cur->next = tok;
  return head.next;
}

// Entry point function of the preprocessor.
Token *preprocess(Token *tok) {
  tok = preprocess2(tok);
  convert_keywords(tok);
  return tok;
}
