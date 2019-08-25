#include "chibi.h"

char *filename;
char *user_input;
Token *token;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error message in the following format.
//
// foo.c:10: x = y + 1;
//               ^ <error message here>
static void verror_at(char *loc, char *fmt, va_list ap) {
  // Find a line containing `loc`.
  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // Get a line number.
  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  // Print out the line.
  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // Show the error message.
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

// Reports an error location and exit.
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
  exit(1);
}

// Reports an error location and exit.
void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->str, fmt, ap);
  exit(1);
}

void warn_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->str, fmt, ap);
}

// Consumes the current token if it matches `op`.
Token *consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      strncmp(token->str, op, token->len))
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

// Returns true if the current token matches a given string.
Token *peek(char *s) {
  if (token->kind != TK_RESERVED || strlen(s) != token->len ||
      strncmp(token->str, s, token->len))
    return NULL;
  return token;
}

// Consumes the current token if it is an identifier.
Token *consume_ident(void) {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

// Ensure that the current token is a given string
void expect(char *s) {
  if (!peek(s))
    error_tok(token, "expected \"%s\"", s);
  token = token->next;
}

// Ensure that the current token is TK_NUM.
long expect_number(void) {
  if (token->kind != TK_NUM)
    error_tok(token, "expected a number");
  long val = token->val;
  token = token->next;
  return val;
}

// Ensure that the current token is TK_IDENT.
char *expect_ident(void) {
  if (token->kind != TK_IDENT)
    error_tok(token, "expected an identifier");
  char *s = strndup(token->str, token->len);
  token = token->next;
  return s;
}

bool at_eof(void) {
  return token->kind == TK_EOF;
}

// Create a new token and add it as the next token of `cur`.
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

static bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

static char *starts_with_reserved(char *p) {
  // Keyword
  static char *kw[] = {"return", "if", "else", "while", "for", "int",
                       "char", "sizeof", "struct", "typedef", "short",
                       "long", "void", "_Bool", "enum", "static", "break",
                       "continue", "goto", "switch", "case", "default",
                       "extern", "_Alignof", "do"};

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
    int len = strlen(kw[i]);
    if (startswith(p, kw[i]) && !is_alnum(p[len]))
      return kw[i];
  }

  // Multi-letter punctuator
  static char *ops[] = {"<<=", ">>=", "...", "==", "!=", "<=", ">=",
                        "->", "++", "--", "<<", ">>", "+=", "-=", "*=",
                        "/=", "&&", "||", "&=", "|=", "^="};

  for (int i = 0; i < sizeof(ops) / sizeof(*ops); i++)
    if (startswith(p, ops[i]))
      return ops[i];

  return NULL;
}

static char get_escape_char(char c) {
  switch (c) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 't': return '\t';
  case 'n': return '\n';
  case 'v': return '\v';
  case 'f': return '\f';
  case 'r': return '\r';
  case 'e': return 27;
  case '0': return 0;
  default: return c;
  }
}

static Token *read_string_literal(Token *cur, char *start) {
  char *p = start + 1;
  char buf[1024];
  int len = 0;

  for (;;) {
    if (len == sizeof(buf))
      error_at(start, "string literal too large");
    if (*p == '\0')
      error_at(start, "unclosed string literal");
    if (*p == '"')
      break;

    if (*p == '\\') {
      p++;
      buf[len++] = get_escape_char(*p++);
    } else {
      buf[len++] = *p++;
    }
  }

  Token *tok = new_token(TK_STR, cur, start, p - start + 1);
  tok->contents = malloc(len + 1);
  memcpy(tok->contents, buf, len);
  tok->contents[len] = '\0';
  tok->cont_len = len + 1;
  return tok;
}

static Token *read_char_literal(Token *cur, char *start) {
  char *p = start + 1;
  if (*p == '\0')
    error_at(start, "unclosed char literal");

  char c;
  if (*p == '\\') {
    p++;
    c = get_escape_char(*p++);
  } else {
    c = *p++;
  }

  if (*p != '\'')
    error_at(start, "char literal too long");
  p++;

  Token *tok = new_token(TK_NUM, cur, start, p - start);
  tok->val = c;
  return tok;
}

static Token *read_int_literal(Token *cur, char *start) {
  char *p = start;

  int base;
  if (!strncasecmp(p, "0x", 2) && is_alnum(p[2])) {
    p += 2;
    base = 16;
  } else if (!strncasecmp(p, "0b", 2) && is_alnum(p[2])) {
    p += 2;
    base = 2;
  } else if (*p == '0') {
    base = 8;
  } else {
    base = 10;
  }

  long val = strtol(p, &p, base);
  if (is_alnum(*p))
    error_at(p, "invalid digit");

  Token *tok = new_token(TK_NUM, cur, start, p - start);
  tok->val = val;
  return tok;
}

// Tokenize `user_input` and returns new tokens.
Token *tokenize(void) {
  char *p = user_input;
  Token head = {};
  Token *cur = &head;

  while (*p) {
    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Skip line comments.
    if (startswith(p, "//")) {
      p += 2;
      while (*p != '\n')
        p++;
      continue;
    }

    // Skip block comments.
    if (startswith(p, "/*")) {
      char *q = strstr(p + 2, "*/");
      if (!q)
        error_at(p, "unclosed block comment");
      p = q + 2;
      continue;
    }

    // String literal
    if (*p == '"') {
      cur = read_string_literal(cur, p);
      p += cur->len;
      continue;
    }

    // Character literal
    if (*p == '\'') {
      cur = read_char_literal(cur, p);
      p += cur->len;
      continue;
    }

    // Keywords or multi-letter punctuators
    char *kw = starts_with_reserved(p);
    if (kw) {
      int len = strlen(kw);
      cur = new_token(TK_RESERVED, cur, p, len);
      p += len;
      continue;
    }

    // Identifier
    if (is_alpha(*p)) {
      char *q = p++;
      while (is_alnum(*p))
        p++;
      cur = new_token(TK_IDENT, cur, q, p - q);
      continue;
    }

    // Single-letter punctuators
    if (ispunct(*p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Integer literal
    if (isdigit(*p)) {
      cur = read_int_literal(cur, p);
      p += cur->len;
      continue;
    }

    error_at(p, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
