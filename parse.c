#include "chibicc.h"

// Scope for local variables, global variables or typedefs
typedef struct VarScope VarScope;
struct VarScope {
  VarScope *next;
  char *name;
  Var *var;
  Type *type_def;
  Type *enum_ty;
  int enum_val;
};

// Scope for struct tags
typedef struct TagScope TagScope;
struct TagScope {
  TagScope *next;
  char *name;
  Type *ty;
};

VarList *locals;
VarList *globals;

VarScope *var_scope;
TagScope *tag_scope;

// Find a variable or a typedef by name.
VarScope *find_var(Token *tok) {
  for (VarScope *sc = var_scope; sc; sc = sc->next) {
    if (strlen(sc->name) == tok->len && !memcmp(tok->str, sc->name, tok->len))
      return sc;
  }
  return NULL;
}

TagScope *find_tag(Token *tok) {
  for (TagScope *sc = tag_scope; sc; sc = sc->next)
    if (strlen(sc->name) == tok->len && !memcmp(tok->str, sc->name, tok->len))
      return sc;
  return NULL;
}

Node *new_node(NodeKind kind, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

Node *new_num(long val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);
  node->val = val;
  return node;
}

Node *new_var(Var *var, Token *tok) {
  Node *node = new_node(ND_VAR, tok);
  node->var = var;
  return node;
}

VarScope *push_scope(char *name) {
  VarScope *sc = calloc(1, sizeof(VarScope));
  sc->name = name;
  sc->next = var_scope;
  var_scope = sc;
  return sc;
}

Var *push_var(char *name, Type *ty, bool is_local) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->ty = ty;
  var->is_local = is_local;

  VarList *vl = calloc(1, sizeof(VarList));
  vl->var = var;
  if (is_local) {
    vl->next = locals;
    locals = vl;
  } else if (ty->kind != TY_FUNC) {
    vl->next = globals;
    globals = vl;
  }
  return var;
}

Type *find_typedef(Token *tok) {
  if (tok->kind == TK_IDENT) {
    VarScope *sc = find_var(token);
    if (sc)
      return sc->type_def;
  }
  return NULL;
}

char *new_label() {
  static int cnt = 0;
  char buf[20];
  sprintf(buf, ".L.data.%d", cnt++);
  return strndup(buf, 20);
}

Function *function();
Type *type_specifier();
Type *declarator(Type *ty, char **name);
Type *abstract_declarator(Type *ty);
Type *type_suffix(Type *ty);
Type *type_name();
Type *struct_decl();
Type *enum_specifier();
Member *struct_member();
void global_var();
Node *declaration();
bool is_typename();
Node *stmt();
Node *expr();
Node *assign();
Node *bitand();
Node *bitor();
Node *bitxor();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *cast();
Node *unary();
Node *postfix();
Node *primary();

bool is_function() {
  Token *tok = token;

  Type *ty = type_specifier();
  char *name = NULL;
  declarator(ty, &name);
  bool isfunc = name && consume("(");

  token = tok;
  return isfunc;
}

// program = (global-var | function)*
Program *program() {
  Function head;
  head.next = NULL;
  Function *cur = &head;
  globals = NULL;

  while (!at_eof()) {
    if (is_function()) {
      Function *fn = function();
      if (!fn)
        continue;
      cur->next = fn;
      cur = cur->next;
      continue;
    }

    global_var();
  }

  Program *prog = calloc(1, sizeof(Program));
  prog->globals = globals;
  prog->fns = head.next;
  return prog;
}

// type-specifier = builtin-type | struct-decl | typedef-name | enum-specifier
//
// builtin-type   = "void"
//                | "_Bool"
//                | "char"
//                | "short" | "short" "int" | "int" "short"
//                | "int"
//                | "long" | "long" "int" | "int" "long"
//
// Note that "typedef" and "static" can appear anywhere in a type-specifier.
Type *type_specifier() {
  if (!is_typename(token))
    error_tok(token, "typename expected");

  Type *ty = NULL;

  enum {
    VOID = 1 << 1,
    BOOL = 1 << 3,
    CHAR = 1 << 5,
    SHORT = 1 << 7,
    INT = 1 << 9,
    LONG = 1 << 11,
  };

  int base_type = 0;
  Type *user_type = NULL;

  bool is_typedef = false;
  bool is_static = false;

  for (;;) {
    // Read one token at a time.
    Token *tok = token;
    if (consume("typedef")) {
      is_typedef = true;
    } else if (consume("static")) {
      is_static = true;
    } else if (consume("void")) {
      base_type += VOID;
    } else if (consume("_Bool")) {
      base_type += BOOL;
    } else if (consume("char")) {
      base_type += CHAR;
    } else if (consume("short")) {
      base_type += SHORT;
    } else if (consume("int")) {
      base_type += INT;
    } else if (consume("long")) {
      base_type += LONG;
    } else if (peek("struct")) {
      if (base_type || user_type)
        break;
      user_type = struct_decl();
    } else if (peek("enum")) {
      if (base_type || user_type)
        break;
      user_type = enum_specifier();
    } else {
      if (base_type || user_type)
        break;
      Type *ty = find_typedef(token);
      if (!ty)
        break;
      token = token->next;
      user_type = ty;
    }

    switch (base_type) {
    case VOID:
      ty = void_type();
      break;
    case BOOL:
      ty = bool_type();
      break;
    case CHAR:
      ty = char_type();
      break;
    case SHORT:
    case SHORT + INT:
      ty = short_type();
      break;
    case INT:
      ty = int_type();
      break;
    case LONG:
    case LONG + INT:
      ty = long_type();
      break;
    case 0:
      // If there's no type specifier, it becomes int.
      // For example, `typedef x` defines x as an alias for int.
      ty = user_type ? user_type : int_type();
      break;
    default:
      error_tok(tok, "invalid type");
    }
  }

  ty->is_typedef = is_typedef;
  ty->is_static = is_static;
  return ty;
}

// declarator = "*"* ("(" declarator ")" | ident) type-suffix
Type *declarator(Type *ty, char **name) {
  while (consume("*"))
    ty = pointer_to(ty);

  if (consume("(")) {
    Type *placeholder = calloc(1, sizeof(Type));
    Type *new_ty = declarator(placeholder, name);
    expect(")");
    *placeholder = *type_suffix(ty);
    return new_ty;
  }

  *name = expect_ident();
  return type_suffix(ty);
}

// abstract-declarator = "*"* ("(" abstract-declarator ")")? type-suffix
Type *abstract_declarator(Type *ty) {
  while (consume("*"))
    ty = pointer_to(ty);

  if (consume("(")) {
    Type *placeholder = calloc(1, sizeof(Type));
    Type *new_ty = abstract_declarator(placeholder);
    expect(")");
    *placeholder = *type_suffix(ty);
    return new_ty;
  }
  return type_suffix(ty);
}

// type-suffix = ("[" num "]" type-suffix)?
Type *type_suffix(Type *ty) {
  if (!consume("["))
    return ty;
  int sz = expect_number();
  expect("]");
  ty = type_suffix(ty);
  return array_of(ty, sz);
}

// type-name = type-specifier abstract-declarator type-suffix
Type *type_name() {
  Type *ty = type_specifier();
  ty = abstract_declarator(ty);
  return type_suffix(ty);
}

void push_tag_scope(Token *tok, Type *ty) {
  TagScope *sc = calloc(1, sizeof(TagScope));
  sc->next = tag_scope;
  sc->name = strndup(tok->str, tok->len);
  sc->ty = ty;
  tag_scope = sc;
}

// struct-decl = "struct" ident
//             | "struct" ident? "{" struct-member "}"
Type *struct_decl() {
  // Read a struct tag.
  expect("struct");
  Token *tag = consume_ident();
  if (tag && !peek("{")) {
    TagScope *sc = find_tag(tag);
    if (!sc)
      error_tok(tag, "unknown struct type");
    if (sc->ty->kind != TY_STRUCT)
      error_tok(tag, "not a struct tag");
    return sc->ty;
  }

  expect("{");

  // Read struct members.
  Member head;
  head.next = NULL;
  Member *cur = &head;

  while (!consume("}")) {
    cur->next = struct_member();
    cur = cur->next;
  }

  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_STRUCT;
  ty->members = head.next;

  // Assign offsets within the struct to members.
  int offset = 0;
  for (Member *mem = ty->members; mem; mem = mem->next) {
    offset = align_to(offset, mem->ty->align);
    mem->offset = offset;
    offset += size_of(mem->ty);

    if (ty->align < mem->ty->align)
      ty->align = mem->ty->align;
  }

  // Register the struct type if a name was given.
  if (tag)
    push_tag_scope(tag, ty);
  return ty;
}

// enum-specifier = "enum" ident
//                | "enum" ident? "{" enum-list? "}"
//
// enum-list = ident ("=" num)? ("," ident ("=" num)?)* ","?
Type *enum_specifier() {
  expect("enum");
  Type *ty = enum_type();

  // Read an enum tag.
  Token *tag = consume_ident();
  if (tag && !peek("{")) {
    TagScope *sc = find_tag(tag);
    if (!sc)
      error_tok(tag, "unknown enum type");
    if (sc->ty->kind != TY_ENUM)
      error_tok(tag, "not an enum tag");
    return sc->ty;
  }

  expect("{");

  // Read enum-list.
  int cnt = 0;
  for (;;) {
    char *name = expect_ident();
    if (consume("="))
      cnt = expect_number();

    VarScope *sc = push_scope(name);
    sc->enum_ty = ty;
    sc->enum_val = cnt++;

    if (consume(",")) {
      if (consume("}"))
        break;
      continue;
    }
    expect("}");
    break;
  }

  if (tag)
    push_tag_scope(tag, ty);
  return ty;
}

// struct-member = type-specifier declarator type-suffix ";"
Member *struct_member() {
  Type *ty = type_specifier();
  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);
  expect(";");

  Member *mem = calloc(1, sizeof(Member));
  mem->name = name;
  mem->ty = ty;
  return mem;
}

VarList *read_func_param() {
  Type *ty = type_specifier();
  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);

  Var *var = push_var(name, ty, true);
  push_scope(name)->var = var;

  VarList *vl = calloc(1, sizeof(VarList));
  vl->var = var;
  return vl;
}

VarList *read_func_params() {
  if (consume(")"))
    return NULL;

  VarList *head = read_func_param();
  VarList *cur = head;

  while (!consume(")")) {
    expect(",");
    cur->next = read_func_param();
    cur = cur->next;
  }

  return head;
}

// function = type-specifier declarator "(" params? ")" ("{" stmt* "}" | ";")
// params   = param ("," param)*
// param    = type-specifier declarator type-suffix
Function *function() {
  locals = NULL;

  Type *ty = type_specifier();
  char *name = NULL;
  ty = declarator(ty, &name);

  // Add a function type to the scope
  Var *var = push_var(name, func_type(ty), false);
  push_scope(name)->var = var;

  // Construct a function object
  Function *fn = calloc(1, sizeof(Function));
  fn->name = name;
  expect("(");
  fn->params = read_func_params();

  if (consume(";"))
    return NULL;

  // Read function body
  Node head;
  head.next = NULL;
  Node *cur = &head;
  expect("{");
  while (!consume("}")) {
    cur->next = stmt();
    cur = cur->next;
  }

  fn->node = head.next;
  fn->locals = locals;
  return fn;
}

// global-var = type-specifier declarator type-suffix ";"
void global_var() {
  Type *ty = type_specifier();
  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);
  expect(";");

  Var *var = push_var(name, ty, false);
  push_scope(name)->var = var;
}

// declaration = type-specifier declarator type-suffix ("=" expr)? ";"
//             | type-specifier ";"
Node *declaration() {
  Token *tok = token;
  Type *ty = type_specifier();
  if (consume(";"))
    return new_node(ND_NULL, tok);

  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);

  if (ty->is_typedef) {
    expect(";");
    ty->is_typedef = false;
    push_scope(name)->type_def = ty;
    return new_node(ND_NULL, tok);
  }

  if (ty->kind == TY_VOID)
    error_tok(tok, "variable declared void");

  Var *var;
  if (ty->is_static)
    var = push_var(new_label(), ty, false);
  else
    var = push_var(name, ty, true);
  push_scope(name)->var = var;

  if (consume(";"))
    return new_node(ND_NULL, tok);

  expect("=");

  Node *lhs = new_var(var, tok);
  Node *rhs = expr();
  expect(";");
  Node *node = new_binary(ND_ASSIGN, lhs, rhs, tok);
  return new_unary(ND_EXPR_STMT, node, tok);
}

Node *read_expr_stmt() {
  Token *tok = token;
  return new_unary(ND_EXPR_STMT, expr(), tok);
}

bool is_typename() {
  return peek("void") || peek("_Bool") || peek("char") || peek("short") ||
         peek("int") || peek("long") || peek("enum") || peek("struct") ||
         peek("typedef") || peek("static") || find_typedef(token);
}

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" (expr? ";" | declaration) expr? ";" expr? ")" stmt
//      | "{" stmt* "}"
//      | declaration
//      | expr ";"
Node *stmt() {
  Token *tok;
  if (tok = consume("return")) {
    Node *node = new_unary(ND_RETURN, expr(), tok);
    expect(";");
    return node;
  }

  if (tok = consume("if")) {
    Node *node = new_node(ND_IF, tok);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else"))
      node->els = stmt();
    return node;
  }

  if (tok = consume("while")) {
    Node *node = new_node(ND_WHILE, tok);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if (tok = consume("for")) {
    Node *node = new_node(ND_FOR, tok);
    expect("(");

    VarScope *sc1 = var_scope;
    TagScope *sc2 = tag_scope;

    if (!consume(";")) {
      if (is_typename()) {
        node->init = declaration();
      } else {
        node->init = read_expr_stmt();
        expect(";");
      }
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = read_expr_stmt();
      expect(")");
    }
    node->then = stmt();

    var_scope = sc1;
    tag_scope = sc2;
    return node;
  }

  if (tok = consume("{")) {
    Node head;
    head.next = NULL;
    Node *cur = &head;

    VarScope *sc1 = var_scope;
    TagScope *sc2 = tag_scope;
    while (!consume("}")) {
      cur->next = stmt();
      cur = cur->next;
    }
    var_scope = sc1;
    tag_scope = sc2;

    Node *node = new_node(ND_BLOCK, tok);
    node->body = head.next;
    return node;
  }

  if (is_typename())
    return declaration();

  Node *node = read_expr_stmt();
  expect(";");
  return node;
}

// expr = assign ("," assign)*
Node *expr() {
  Node *node = assign();
  Token *tok;
  while (tok = consume(",")) {
    node = new_unary(ND_EXPR_STMT, node, node->tok);
    node = new_binary(ND_COMMA, node, assign(), tok);
  }
  return node;
}

// assign    = bitor (assign-op assign)?
// assign-op = "=" | "+=" | "-=" | "*=" | "/="
Node *assign() {
  Node *node = bitor();
  Token *tok;
  if (tok = consume("="))
    node = new_binary(ND_ASSIGN, node, assign(), tok);
  if (tok = consume("+="))
    node = new_binary(ND_A_ADD, node, assign(), tok);
  if (tok = consume("-="))
    node = new_binary(ND_A_SUB, node, assign(), tok);
  if (tok = consume("*="))
    node = new_binary(ND_A_MUL, node, assign(), tok);
  if (tok = consume("/="))
    node = new_binary(ND_A_DIV, node, assign(), tok);
  return node;
}

// bitor = bitxor ("|" bitxor)*
Node *bitor() {
  Node *node = bitxor();
  Token *tok;
  while (tok = consume("|"))
    node = new_binary(ND_BITOR, node, bitxor(), tok);
  return node;
}

// bitxor = bitand ("^" bitand)*
Node *bitxor() {
  Node *node = bitand();
  Token *tok;
  while (tok = consume("^"))
    node = new_binary(ND_BITXOR, node, bitxor(), tok);
  return node;
}

// bitand = equality ("&" equality)*
Node *bitand() {
  Node *node = equality();
  Token *tok;
  while (tok = consume("&"))
    node = new_binary(ND_BITAND, node, equality(), tok);
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();
  Token *tok;

  for (;;) {
    if (tok = consume("=="))
      node = new_binary(ND_EQ, node, relational(), tok);
    else if (tok = consume("!="))
      node = new_binary(ND_NE, node, relational(), tok);
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();
  Token *tok;

  for (;;) {
    if (tok = consume("<"))
      node = new_binary(ND_LT, node, add(), tok);
    else if (tok = consume("<="))
      node = new_binary(ND_LE, node, add(), tok);
    else if (tok = consume(">"))
      node = new_binary(ND_LT, add(), node, tok);
    else if (tok = consume(">="))
      node = new_binary(ND_LE, add(), node, tok);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();
  Token *tok;

  for (;;) {
    if (tok = consume("+"))
      node = new_binary(ND_ADD, node, mul(), tok);
    else if (tok = consume("-"))
      node = new_binary(ND_SUB, node, mul(), tok);
    else
      return node;
  }
}

// mul = cast ("*" cast | "/" cast)*
Node *mul() {
  Node *node = cast();
  Token *tok;

  for (;;) {
    if (tok = consume("*"))
      node = new_binary(ND_MUL, node, cast(), tok);
    else if (tok = consume("/"))
      node = new_binary(ND_DIV, node, cast(), tok);
    else
      return node;
  }
}

// cast = "(" type-name ")" cast | unary
Node *cast() {
  Token *tok = token;

  if (consume("(")) {
    if (is_typename()) {
      Type *ty = type_name();
      expect(")");
      Node *node = new_unary(ND_CAST, cast(), tok);
      node->ty = ty;
      return node;
    }
    token = tok;
  }

  return unary();
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~")? cast
//       | ("++" | "--") unary
//       | postfix
Node *unary() {
  Token *tok;
  if (consume("+"))
    return cast();
  if (tok = consume("-"))
    return new_binary(ND_SUB, new_num(0, tok), cast(), tok);
  if (tok = consume("&"))
    return new_unary(ND_ADDR, cast(), tok);
  if (tok = consume("*"))
    return new_unary(ND_DEREF, cast(), tok);
  if (tok = consume("!"))
    return new_unary(ND_NOT, cast(), tok);
  if (tok = consume("~"))
    return new_unary(ND_BITNOT, cast(), tok);
  if (tok = consume("++"))
    return new_unary(ND_PRE_INC, unary(), tok);
  if (tok = consume("--"))
    return new_unary(ND_PRE_DEC, unary(), tok);
  return postfix();
}

// postfix = primary ("[" expr "]" | "." ident | "->" ident | "++" | "--")*
Node *postfix() {
  Node *node = primary();
  Token *tok;

  for (;;) {
    if (tok = consume("[")) {
      // x[y] is short for *(x+y)
      Node *exp = new_binary(ND_ADD, node, expr(), tok);
      expect("]");
      node = new_unary(ND_DEREF, exp, tok);
      continue;
    }

    if (tok = consume(".")) {
      node = new_unary(ND_MEMBER, node, tok);
      node->member_name = expect_ident();
      continue;
    }

    if (tok = consume("->")) {
      // x->y is short for (*x).y
      node = new_unary(ND_DEREF, node, tok);
      node = new_unary(ND_MEMBER, node, tok);
      node->member_name = expect_ident();
      continue;
    }

    if (tok = consume("++")) {
      node = new_unary(ND_POST_INC, node, tok);
      continue;
    }

    if (tok = consume("--")) {
      node = new_unary(ND_POST_DEC, node, tok);
      continue;
    }

    return node;
  }
}

// stmt-expr = "(" "{" stmt stmt* "}" ")"
//
// Statement expression is a GNU C extension.
Node *stmt_expr(Token *tok) {
  VarScope *sc1 = var_scope;
  TagScope *sc2 = tag_scope;

  Node *node = new_node(ND_STMT_EXPR, tok);
  node->body = stmt();
  Node *cur = node->body;

  while (!consume("}")) {
    cur->next = stmt();
    cur = cur->next;
  }
  expect(")");

  var_scope = sc1;
  tag_scope = sc2;

  if (cur->kind != ND_EXPR_STMT)
    error_tok(cur->tok, "stmt expr returning void is not supported");
  *cur = *cur->lhs;
  return node;
}

// func-args = "(" (assign ("," assign)*)? ")"
Node *func_args() {
  if (consume(")"))
    return NULL;

  Node *head = assign();
  Node *cur = head;
  while (consume(",")) {
    cur->next = assign();
    cur = cur->next;
  }
  expect(")");
  return head;
}

// primary = "(" "{" stmt-expr-tail
//         | "(" expr ")"
//         | "sizeof" "(" type-name ")"
//         | "sizeof" unary
//         | ident func-args?
//         | str
//         | num
Node *primary() {
  Token *tok;

  if (tok = consume("(")) {
    if (consume("{"))
      return stmt_expr(tok);

    Node *node = expr();
    expect(")");
    return node;
  }

  if (tok = consume("sizeof")) {
    if (consume("(")) {
      if (is_typename()) {
        Type *ty = type_name();
        expect(")");
        return new_num(size_of(ty), tok);
      }
      token = tok->next;
    }
    return new_unary(ND_SIZEOF, unary(), tok);
  }

  if (tok = consume_ident()) {
    if (consume("(")) {
      Node *node = new_node(ND_FUNCALL, tok);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args();

      VarScope *sc = find_var(tok);
      if (sc) {
        if (!sc->var || sc->var->ty->kind != TY_FUNC)
          error_tok(tok, "not a function");
        node->ty = sc->var->ty->return_ty;
      } else {
        node->ty = int_type();
      }
      return node;
    }

    VarScope *sc = find_var(tok);
    if (sc) {
      if (sc->var)
        return new_var(sc->var, tok);
      if (sc->enum_ty)
        return new_num(sc->enum_val, tok);
    }
    error_tok(tok, "undefined variable");
  }

  tok = token;
  if (tok->kind == TK_STR) {
    token = token->next;

    Type *ty = array_of(char_type(), tok->cont_len);
    Var *var = push_var(new_label(), ty, false);
    var->contents = tok->contents;
    var->cont_len = tok->cont_len;
    return new_var(var, tok);
  }

  if (tok->kind != TK_NUM)
    error_tok(tok, "expected expression");
  return new_num(expect_number(), tok);
}
