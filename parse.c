#include "chibicc.h"

// All local variable instances created during parsing are
// accumulated to this list.
Var *locals;

static Node *compound_stmt(Token **rest, Token *tok);
static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

// Find a local variable by name.
static Var *find_var(Token *tok) {
  for (Var *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len && !strncmp(tok->loc, var->name, tok->len))
      return var;
  return NULL;
}

static Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

static Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

static Node *new_var_node(Var *var) {
  Node *node = new_node(ND_VAR);
  node->var = var;
  return node;
}

static Var *new_lvar(char *name) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->next = locals;
  locals = var;
  return var;
}

// stmt = "return" expr ";"
//      | "{" compound-stmt
//      | expr-stmt
static Node *stmt(Token **rest, Token *tok) {
  if (equal(tok, "return")) {
    Node *node = new_unary(ND_RETURN, expr(&tok, tok->next));
    *rest = skip(tok, ";");
    return node;
  }

  if (equal(tok, "{"))
    return compound_stmt(rest, tok->next);

  return expr_stmt(rest, tok);
}

// compound-stmt = stmt* "}"
static Node *compound_stmt(Token **rest, Token *tok) {
  Node head = {};
  Node *cur = &head;
  while (!equal(tok, "}"))
    cur = cur->next = stmt(&tok, tok);

  Node *node = new_node(ND_BLOCK);
  node->body = head.next;
  *rest = tok->next;
  return node;
}

// expr-stmt = expr? ";"
static Node *expr_stmt(Token **rest, Token *tok) {
  if (equal(tok, ";")) {
    *rest = tok->next;
    return new_node(ND_BLOCK);
  }

  Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
  *rest = skip(tok, ";");
  return node;
}

// expr = assign
static Node *expr(Token **rest, Token *tok) {
  return assign(rest, tok);
}

// assign = equality ("=" assign)?
static Node *assign(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);
  if (equal(tok, "="))
    node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);

  for (;;) {
    if (equal(tok, "==")) {
      Node *rhs = relational(&tok, tok->next);
      node = new_binary(ND_EQ, node, rhs);
      continue;
    }

    if (equal(tok, "!=")) {
      Node *rhs = relational(&tok, tok->next);
      node = new_binary(ND_NE, node, rhs);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  for (;;) {
    if (equal(tok, "<")) {
      Node *rhs = add(&tok, tok->next);
      node = new_binary(ND_LT, node, rhs);
      continue;
    }

    if (equal(tok, "<=")) {
      Node *rhs = add(&tok, tok->next);
      node = new_binary(ND_LE, node, rhs);
      continue;
    }

    if (equal(tok, ">")) {
      Node *rhs = add(&tok, tok->next);
      node = new_binary(ND_LT, rhs, node);
      continue;
    }

    if (equal(tok, ">=")) {
      Node *rhs = add(&tok, tok->next);
      node = new_binary(ND_LE, rhs, node);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      Node *rhs = mul(&tok, tok->next);
      node = new_binary(ND_ADD, node, rhs);
      continue;
    }

    if (equal(tok, "-")) {
      Node *rhs = mul(&tok, tok->next);
      node = new_binary(ND_SUB, node, rhs);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul(Token **rest, Token *tok) {
  Node *node = unary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      Node *rhs = unary(&tok, tok->next);
      node = new_binary(ND_MUL, node, rhs);
      continue;
    }

    if (equal(tok, "/")) {
      Node *rhs = unary(&tok, tok->next);
      node = new_binary(ND_DIV, node, rhs);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// unary = ("+" | "-") unary
//       | primary
static Node *unary(Token **rest, Token *tok) {
  if (equal(tok, "+"))
    return unary(rest, tok->next);

  if (equal(tok, "-"))
    return new_binary(ND_SUB, new_num(0), unary(rest, tok->next));

  return primary(rest, tok);
}

// primary = "(" expr ")" | ident | num
static Node *primary(Token **rest, Token *tok) {
  if (equal(tok, "(")) {
    Node *node = expr(&tok, tok->next);
    *rest = skip(tok, ")");
    return node;
  }

  if (tok->kind == TK_IDENT) {
    Var *var = find_var(tok);
    if (!var)
      var = new_lvar(strndup(tok->loc, tok->len));
    *rest = tok->next;
    return new_var_node(var);
  }

  if (tok->kind == TK_NUM) {
    Node *node = new_num(tok->val);
    *rest = tok->next;
    return node;
  }

  error_tok(tok, "expected an expression");
}

// program = stmt*
Function *parse(Token *tok) {
  tok = skip(tok, "{");

  Function *prog = calloc(1, sizeof(Function));
  prog->body = compound_stmt(&tok, tok);
  prog->locals = locals;
  return prog;
}
