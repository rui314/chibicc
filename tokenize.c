#include "chibicc.h"

// Input filename
static char *current_filename;

// Input string
static char *current_input;

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
static void verror_at(int line_no, char *loc, char *fmt, va_list ap) {
  // Find a line containing `loc`.
  char *line = loc;
  while (current_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // Print out the line.
  int indent = fprintf(stderr, "%s:%d: ", current_filename, line_no);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // Show the error message.
  int pos = loc - line + indent;

  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

void error_at(char *loc, char *fmt, ...) {
  int line_no = 1;
  for (char *p = current_input; p < loc; p++)
    if (*p == '\n')
      line_no++;

  va_list ap;
  va_start(ap, fmt);
  verror_at(line_no, loc, fmt, ap);
  exit(1);
}

void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->line_no, tok->loc, fmt, ap);
  exit(1);
}

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op) {
  return strlen(op) == tok->len &&
         !strncmp(tok->loc, op, tok->len);
}

// Ensure that the current token is `op`.
Token *skip(Token *tok, char *op) {
  if (!equal(tok, op))
    error_tok(tok, "expected '%s'", op);
  return tok->next;
}

bool consume(Token **rest, Token *tok, char *str) {
  if (equal(tok, str)) {
    *rest = tok->next;
    return true;
  }
  *rest = tok;
  return false;
}

// Create a new token.
static Token *new_token(TokenKind kind, char *start, char *end) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start;
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Returns true if c is valid as the first character of an identifier.
static bool is_ident1(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// Returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c) {
  return is_ident1(c) || ('0' <= c && c <= '9');
}

static int from_hex(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

static bool is_keyword(Token *tok) {
  static char *kw[] = {
    "return", "if", "else", "for", "while", "int", "sizeof", "char",
    "struct", "union", "short", "long", "void", "typedef", "_Bool",
    "enum", "static", "goto", "break", "continue", "switch", "case",
    "default", "extern", "_Alignof", "_Alignas", "do", "signed",
    "unsigned",
  };

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
    if (equal(tok, kw[i]))
      return true;
  return false;
}

static int read_escaped_char(char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;
    return c;
  }

  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p))
      error_at(p, "invalid hex escape sequence");

    int c = 0;
    for (; isxdigit(*p); p++)
      c = (c << 4) + from_hex(*p);
    *new_pos = p;
    return c;
  }

  *new_pos = p + 1;

  switch (*p) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 't': return '\t';
  case 'n': return '\n';
  case 'v': return '\v';
  case 'f': return '\f';
  case 'r': return '\r';
  // [GNU] \e for the ASCII escape character is a GNU C extension.
  case 'e': return 27;
  default: return *p;
  }
}

// Find a closing double-quote.
static char *string_literal_end(char *p) {
  char *start = p;
  for (; *p != '"'; p++) {
    if (*p == '\0')
      error_at(start, "unclosed string literal");
    if (*p == '\\')
      p++;
  }
  return p;
}

static Token *read_string_literal(char *start) {
  char *end = string_literal_end(start + 1);
  char *buf = calloc(1, end - start);
  int len = 0;

  for (char *p = start + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(&p, p + 1);
    else
      buf[len++] = *p++;
  }

  Token *tok = new_token(TK_STR, start, end + 1);
  tok->ty = array_of(ty_char, len + 1);
  tok->str = buf;
  return tok;
}

static Token *read_char_literal(char *start) {
  char *p = start + 1;
  if (*p == '\0')
    error_at(start, "unclosed char literal");

  char c;
  if (*p == '\\')
    c = read_escaped_char(&p, p + 1);
  else
    c = *p++;

  char *end = strchr(p, '\'');
  if (!end)
    error_at(p, "unclosed char literal");

  Token *tok = new_token(TK_NUM, start, end + 1);
  tok->val = c;
  return tok;
}

static Token *read_int_literal(char *start) {
  char *p = start;

  int base = 10;
  if (!strncasecmp(p, "0x", 2) && isalnum(p[2])) {
    p += 2;
    base = 16;
  } else if (!strncasecmp(p, "0b", 2) && isalnum(p[2])) {
    p += 2;
    base = 2;
  } else if (*p == '0') {
    base = 8;
  }

  int64_t val = strtoul(p, &p, base);
  if (isalnum(*p))
    error_at(p, "invalid digit");

  Token *tok = new_token(TK_NUM, start, p);
  tok->val = val;
  return tok;
}

static void convert_keywords(Token *tok) {
  for (Token *t = tok; t->kind != TK_EOF; t = t->next)
    if (is_keyword(t))
      t->kind = TK_RESERVED;
}

// Initialize line info for all tokens.
static void add_line_numbers(Token *tok) {
  char *p = current_input;
  int n = 1;

  do {
    if (p == tok->loc) {
      tok->line_no = n;
      tok = tok->next;
    }
    if (*p == '\n')
      n++;
  } while (*p++);
}

// Tokenize a given string and returns new tokens.
static Token *tokenize(char *filename, char *p) {
  current_filename = filename;
  current_input = p;
  Token head = {};
  Token *cur = &head;

  while (*p) {
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

    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Numeric literal
    if (isdigit(*p)) {
      cur = cur->next = read_int_literal(p);
      p += cur->len;
      continue;
    }

    // String literal
    if (*p == '"') {
      cur = cur->next = read_string_literal(p);
      p += cur->len;
      continue;
    }

    // Character literal
    if (*p == '\'') {
      cur = cur->next = read_char_literal(p);
      p += cur->len;
      continue;
    }

    // Identifier or keyword
    if (is_ident1(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident2(*p));
      cur = cur->next = new_token(TK_IDENT, start, p);
      continue;
    }

    // Three-letter punctuators
    if (startswith(p, "<<=") || startswith(p, ">>=") ||
        startswith(p, "...")) {
      cur = cur->next = new_token(TK_RESERVED, p, p + 3);
      p += 3;
      continue;
    }

    // Two-letter punctuators
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=") ||
        startswith(p, "->") || startswith(p, "+=") ||
        startswith(p, "-=") || startswith(p, "*=") ||
        startswith(p, "/=") || startswith(p, "++") ||
        startswith(p, "--") || startswith(p, "%=") ||
        startswith(p, "&=") || startswith(p, "|=") ||
        startswith(p, "^=") || startswith(p, "&&") ||
        startswith(p, "||") || startswith(p, "<<") ||
        startswith(p, ">>")) {
      cur = cur->next = new_token(TK_RESERVED, p, p + 2);
      p += 2;
      continue;
    }

    // Single-letter punctuators
    if (ispunct(*p)) {
      cur = cur->next = new_token(TK_RESERVED, p, p + 1);
      p++;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur = cur->next = new_token(TK_EOF, p, p);
  add_line_numbers(head.next);
  convert_keywords(head.next);
  return head.next;
}

// Returns the contents of a given file.
static char *read_file(char *path) {
  FILE *fp;

  if (strcmp(path, "-") == 0) {
    // By convention, read from stdin if a given filename is "-".
    fp = stdin;
  } else {
    fp = fopen(path, "r");
    if (!fp)
      error("cannot open %s: %s", path, strerror(errno));
  }

  int buflen = 4096;
  int nread = 0;
  char *buf = calloc(1, buflen);

  // Read the entire file.
  for (;;) {
    int end = buflen - 2; // extra 2 bytes for the trailing "\n\0"
    int n = fread(buf + nread, 1, end - nread, fp);
    if (n == 0)
      break;
    nread += n;
    if (nread == end) {
      buflen *= 2;
      buf = realloc(buf, buflen);
    }
  }

  if (fp != stdin)
    fclose(fp);

  // Make sure that the last line is properly terminated with '\n'.
  if (nread == 0 || buf[nread - 1] != '\n')
    buf[nread++] = '\n';

  buf[nread] = '\0';
  return buf;
}

Token *tokenize_file(char *path) {
  return tokenize(path, read_file(path));
}
