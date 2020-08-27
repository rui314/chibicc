#include "chibicc.h"

static bool opt_S;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static char *opt_o;

static char *input_path;

static char **tmpfiles;

static void usage(int status) {
  fprintf(stderr, "chibicc [ -o <path> ] <file>\n");
  exit(status);
}

static void parse_args(int argc, char **argv) {
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
      if (!argv[++i])
        usage(1);
      opt_o = argv[i];
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

    if (argv[i][0] == '-' && argv[i][1] != '\0')
      error("unknown argument: %s", argv[i]);

    input_path = argv[i];
  }

  if (!input_path)
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

  if (input)
    args[argc++] = input;

  if (output) {
    args[argc++] = "-o";
    args[argc++] = output;
  }

  run_subprocess(args);
}

static void cc1(void) {
  // Tokenize and parse.
  Token *tok = tokenize_file(input_path);
  Obj *prog = parse(tok);

  // Traverse the AST to emit assembly.
  FILE *out = open_file(opt_o);
  fprintf(out, ".file 1 \"%s\"\n", input_path);
  codegen(prog, out);
}

static void assemble(char *input, char *output) {
  char *cmd[] = {"as", "-c", input, "-o", output, NULL};
  run_subprocess(cmd);
}

int main(int argc, char **argv) {
  atexit(cleanup);
  parse_args(argc, argv);

  if (opt_cc1) {
    cc1();
    return 0;
  }

  char *output;
  if (opt_o)
    output = opt_o;
  else if (opt_S)
    output = replace_extn(input_path, ".s");
  else
    output = replace_extn(input_path, ".o");

  // If -S is given, assembly text is the final output.
  if (opt_S) {
    run_cc1(argc, argv, input_path, output);
    return 0;
  }

  // Otherwise, run the assembler to assemble our output.
  char *tmpfile = create_tmpfile();
  run_cc1(argc, argv, input_path, tmpfile);
  assemble(tmpfile, output);
  return 0;
}
