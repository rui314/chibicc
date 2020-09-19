#include "chibicc.h"

StringArray include_paths;
bool opt_fcommon = true;

static StringArray opt_include;
static bool opt_E;
static bool opt_S;
static bool opt_c;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static char *opt_o;

char *base_file;
static char *output_file;

static StringArray input_paths;

static char **tmpfiles;

static void usage(int status) {
  fprintf(stderr, "chibicc [ -o <path> ] <file>\n");
  exit(status);
}

static bool take_arg(char *arg) {
  char *x[] = {"-o", "-I", "-idirafter", "-include"};

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)
    if (!strcmp(arg, x[i]))
      return true;
  return false;
}

static void add_default_include_paths(char *argv0) {
  // We expect that chibicc-specific include files are installed
  // to ./include relative to argv[0].
  char *buf = calloc(1, strlen(argv0) + 10);
  sprintf(buf, "%s/include", dirname(strdup(argv0)));
  strarray_push(&include_paths, buf);

  // Add standard include paths.
  strarray_push(&include_paths, "/usr/local/include");
  strarray_push(&include_paths, "/usr/include/x86_64-linux-gnu");
  strarray_push(&include_paths, "/usr/include");
}

static void define(char *str) {
  char *eq = strchr(str, '=');
  if (eq)
    define_macro(strndup(str, eq - str), eq + 1);
  else
    define_macro(str, "1");
}

static void parse_args(int argc, char **argv) {
  // Make sure that all command line options that take an argument
  // have an argument.
  for (int i = 1; i < argc; i++)
    if (take_arg(argv[i]))
      if (!argv[++i])
        usage(1);

  StringArray idirafter = {};

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-###")) {
      opt_hash_hash_hash = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
      continue;
    }

    if (!strcmp(argv[i], "--help"))
      usage(0);

    if (!strcmp(argv[i], "-o")) {
      opt_o = argv[++i];
      continue;
    }

    if (!strncmp(argv[i], "-o", 2)) {
      opt_o = argv[i] + 2;
      continue;
    }

    if (!strcmp(argv[i], "-S")) {
      opt_S = true;
      continue;
    }

    if (!strcmp(argv[i], "-fcommon")) {
      opt_fcommon = true;
      continue;
    }

    if (!strcmp(argv[i], "-fno-common")) {
      opt_fcommon = false;
      continue;
    }

    if (!strcmp(argv[i], "-c")) {
      opt_c = true;
      continue;
    }

    if (!strcmp(argv[i], "-E")) {
      opt_E = true;
      continue;
    }

    if (!strncmp(argv[i], "-I", 2)) {
      strarray_push(&include_paths, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-D")) {
      define(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-D", 2)) {
      define(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-U")) {
      undef_macro(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-U", 2)) {
      undef_macro(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-include")) {
      strarray_push(&opt_include, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-cc1-input")) {
      base_file = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-cc1-output")) {
      output_file = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-idirafter")) {
      strarray_push(&idirafter, argv[i++]);
      continue;
    }

    // These options are ignored for now.
    if (!strncmp(argv[i], "-O", 2) ||
        !strncmp(argv[i], "-W", 2) ||
        !strncmp(argv[i], "-g", 2) ||
        !strncmp(argv[i], "-std=", 5) ||
        !strcmp(argv[i], "-ffreestanding") ||
        !strcmp(argv[i], "-fno-builtin") ||
        !strcmp(argv[i], "-fno-omit-frame-pointer") ||
        !strcmp(argv[i], "-fno-stack-protector") ||
        !strcmp(argv[i], "-fno-strict-aliasing") ||
        !strcmp(argv[i], "-m64") ||
        !strcmp(argv[i], "-mno-red-zone") ||
        !strcmp(argv[i], "-w"))
      continue;

    if (argv[i][0] == '-' && argv[i][1] != '\0')
      error("unknown argument: %s", argv[i]);

    strarray_push(&input_paths, argv[i]);
  }

  for (int i = 0; i < idirafter.len; i++)
    strarray_push(&include_paths, idirafter.data[i]);

  if (input_paths.len == 0)
    error("no input files");
}

static FILE *open_file(char *path) {
  if (!path || strcmp(path, "-") == 0)
    return stdout;

  FILE *out = fopen(path, "w");
  if (!out)
    error("cannot open output file: %s: %s", path, strerror(errno));
  return out;
}

static bool endswith(char *p, char *q) {
  int len1 = strlen(p);
  int len2 = strlen(q);
  return (len1 >= len2) && !strcmp(p + len1 - len2, q);
}

// Replace file extension
static char *replace_extn(char *tmpl, char *extn) {
  char *filename = basename(strdup(tmpl));
  int len1 = strlen(filename);
  int len2 = strlen(extn);
  char *buf = calloc(1, len1 + len2 + 2);

  char *dot = strrchr(filename, '.');
  if (dot)
    *dot = '\0';
  sprintf(buf, "%s%s", filename, extn);
  return buf;
}

static void cleanup(void) {
  if (tmpfiles)
    for (int i = 0; tmpfiles[i]; i++)
      unlink(tmpfiles[i]);
}

static char *create_tmpfile(void) {
  char tmpl[] = "/tmp/chibicc-XXXXXX";
  char *path = calloc(1, sizeof(tmpl));
  memcpy(path, tmpl, sizeof(tmpl));

  int fd = mkstemp(path);
  if (fd == -1)
    error("mkstemp failed: %s", strerror(errno));
  close(fd);

  static int len = 2;
  tmpfiles = realloc(tmpfiles, sizeof(char *) * len);
  tmpfiles[len - 2] = path;
  tmpfiles[len - 1] = NULL;
  len++;

  return path;
}

static void run_subprocess(char **argv) {
  // If -### is given, dump the subprocess's command line.
  if (opt_hash_hash_hash) {
    fprintf(stderr, "%s", argv[0]);
    for (int i = 1; argv[i]; i++)
      fprintf(stderr, " %s", argv[i]);
    fprintf(stderr, "\n");
  }

  if (fork() == 0) {
    // Child process. Run a new command.
    execvp(argv[0], argv);
    fprintf(stderr, "exec failed: %s: %s\n", argv[0], strerror(errno));
    _exit(1);
  }

  // Wait for the child process to finish.
  int status;
  while (wait(&status) > 0);
  if (status != 0)
    exit(1);
}

static void run_cc1(int argc, char **argv, char *input, char *output) {
  char **args = calloc(argc + 10, sizeof(char *));
  memcpy(args, argv, argc * sizeof(char *));
  args[argc++] = "-cc1";

  if (input) {
    args[argc++] = "-cc1-input";
    args[argc++] = input;
  }

  if (output) {
    args[argc++] = "-cc1-output";
    args[argc++] = output;
  }

  run_subprocess(args);
}

// Print tokens to stdout. Used for -E.
static void print_tokens(Token *tok) {
  FILE *out = open_file(opt_o ? opt_o : "-");

  int line = 1;
  for (; tok->kind != TK_EOF; tok = tok->next) {
    if (line > 1 && tok->at_bol)
      fprintf(out, "\n");
    if (tok->has_space && !tok->at_bol)
      fprintf(out, " ");
    fprintf(out, "%.*s", tok->len, tok->loc);
    line++;
  }
  fprintf(out, "\n");
}

static Token *must_tokenize_file(char *path) {
  Token *tok = tokenize_file(path);
  if (!tok)
    error("%s: %s", path, strerror(errno));
  return tok;
}

static Token *append_tokens(Token *tok1, Token *tok2) {
  if (!tok1 || tok1->kind == TK_EOF)
    return tok2;

  Token *t = tok1;
  while (t->next->kind != TK_EOF)
    t = t->next;
  t->next = tok2;
  return tok1;
}

static void cc1(void) {
  Token *tok = NULL;

  // Process -include option
  for (int i = 0; i < opt_include.len; i++) {
    char *incl = opt_include.data[i];

    char *path;
    if (file_exists(incl)) {
      path = incl;
    } else {
      path = search_include_paths(incl);
      if (!path)
        error("-include: %s: %s", incl, strerror(errno));
    }

    Token *tok2 = must_tokenize_file(path);
    tok = append_tokens(tok, tok2);
  }

  // Tokenize and parse.
  Token *tok2 = must_tokenize_file(base_file);
  tok = append_tokens(tok, tok2);
  tok = preprocess(tok);

  // If -E is given, print out preprocessed C code as a result.
  if (opt_E) {
    print_tokens(tok);
    return;
  }

  Obj *prog = parse(tok);

  // Open a temporary output buffer.
  char *buf;
  size_t buflen;
  FILE *output_buf = open_memstream(&buf, &buflen);

  // Traverse the AST to emit assembly.
  codegen(prog, output_buf);
  fclose(output_buf);

  // Write the asembly text to a file.
  FILE *out = open_file(output_file);
  fwrite(buf, buflen, 1, out);
  fclose(out);
}

static void assemble(char *input, char *output) {
  char *cmd[] = {"as", "-c", input, "-o", output, NULL};
  run_subprocess(cmd);
}

static void run_linker(StringArray *inputs, char *output) {
  StringArray arr = {};

  strarray_push(&arr, "ld");
  strarray_push(&arr, "-o");
  strarray_push(&arr, output);
  strarray_push(&arr, "-m");
  strarray_push(&arr, "elf_x86_64");
  strarray_push(&arr, "-dynamic-linker");
  strarray_push(&arr, "/lib64/ld-linux-x86-64.so.2");
  strarray_push(&arr, "/usr/lib/x86_64-linux-gnu/crt1.o");
  strarray_push(&arr, "/usr/lib/x86_64-linux-gnu/crti.o");
  strarray_push(&arr, "/usr/lib/gcc/x86_64-linux-gnu/9/crtbegin.o");
  strarray_push(&arr, "-L/usr/lib/gcc/x86_64-linux-gnu/9");
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");
  strarray_push(&arr, "-L/usr/lib64");
  strarray_push(&arr, "-L/lib/x86_64-linux-gnu");
  strarray_push(&arr, "-L/lib64");
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");
  strarray_push(&arr, "-L/usr/lib");
  strarray_push(&arr, "-L/lib");

  for (int i = 0; i < inputs->len; i++)
    strarray_push(&arr, inputs->data[i]);

  strarray_push(&arr, "-lc");
  strarray_push(&arr, "-lgcc");
  strarray_push(&arr, "--as-needed");
  strarray_push(&arr, "-lgcc_s");
  strarray_push(&arr, "--no-as-needed");
  strarray_push(&arr, "/usr/lib/gcc/x86_64-linux-gnu/9/crtend.o");
  strarray_push(&arr, "/usr/lib/x86_64-linux-gnu/crtn.o");
  strarray_push(&arr, NULL);

  run_subprocess(arr.data);
}

int main(int argc, char **argv) {
  atexit(cleanup);
  init_macros();
  parse_args(argc, argv);

  if (opt_cc1) {
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }

  if (input_paths.len > 1 && opt_o && (opt_c || opt_S | opt_E))
    error("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");

  StringArray ld_args = {};

  for (int i = 0; i < input_paths.len; i++) {
    char *input = input_paths.data[i];

    char *output;
    if (opt_o)
      output = opt_o;
    else if (opt_S)
      output = replace_extn(input, ".s");
    else
      output = replace_extn(input, ".o");

    // Handle .o
    if (endswith(input, ".o")) {
      strarray_push(&ld_args, input);
      continue;
    }

    // Handle .s
    if (endswith(input, ".s")) {
      if (!opt_S)
        assemble(input, output);
      continue;
    }

    // Handle .c
    if (!endswith(input, ".c") && strcmp(input, "-"))
      error("unknown file extension: %s", input);

    // Just preprocess
    if (opt_E) {
      run_cc1(argc, argv, input, NULL);
      continue;
    }

    // Compile
    if (opt_S) {
      run_cc1(argc, argv, input, output);
      continue;
    }

    // Compile and assemble
    if (opt_c) {
      char *tmp = create_tmpfile();
      run_cc1(argc, argv, input, tmp);
      assemble(tmp, output);
      continue;
    }

    // Compile, assemble and link
    char *tmp1 = create_tmpfile();
    char *tmp2 = create_tmpfile();
    run_cc1(argc, argv, input, tmp1);
    assemble(tmp1, tmp2);
    strarray_push(&ld_args, tmp2);
    continue;
  }

  if (ld_args.len > 0)
    run_linker(&ld_args, opt_o ? opt_o : "a.out");
  return 0;
}
