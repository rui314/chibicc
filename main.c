#include "chibicc.h"
#define MAIN_C "main.c"

extern char **environ;

typedef enum
{
  FILE_NONE,
  FILE_C,
  FILE_ASM,
  FILE_OBJ,
  FILE_AR,
  FILE_DSO
} FileType;

StringArray include_paths;
bool opt_fcommon = true;
bool opt_fbuiltin = true;
bool opt_fpic;

static FileType opt_x;
static StringArray opt_include;
static bool opt_E;
static bool opt_M;
static bool opt_MD;
static bool opt_MMD;
static bool opt_MP;
static bool opt_S;
static bool opt_c;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static bool opt_static;
static bool opt_shared;
static bool isCc1input = false;
static bool isCc1output = false;
static bool isDebug = false;
static char *opt_MF;
static char *opt_MT;
char *opt_o;
static char *opt_linker;
// static char *symbolic_name;
static char *r_path;

static StringArray ld_extra_args;
static StringArray std_include_paths;

char *base_file;
static char *output_file;
static FILE *f;

// for dot diagrams
FILE *dotf;
char *dot_file;
bool isDotfile = false;

static char logFile[] = "/tmp/chibicc.log";
static StringArray input_paths;
static StringArray tmpfiles;

static void usage(int status)
{
  fprintf(stderr, HELP);
  fprintf(stderr, USAGE);
  exit(status);
}

// print the version of the command
static void printVersion(int status)
{
  printf("%s version : %s\n", PRODUCT, VERSION);
  exit(status);
}

// check the length and validity of parameter to avoid non valid input values
static void check_parms_length(char *arg)
{
  if (strlen(arg) > MAXLEN)
  {
    error("%s : in check_parms_length maximum length parameter overpassed", MAIN_C);
    exit(EXIT_FAILURE);
  }
}

static bool take_arg(char *arg)
{
  char *x[] = {
      "-o", "-I", "-idirafter", "-include", "-x", "-MF", "-MT", "-MQ", "-Xlinker", "-cc1-input", "-cc1-output", "-fuse-ld", "-soname", "-rpath", "-z", "--version-script"};

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)
  {
    if (!strcmp(arg, x[i]))
      return true;
  }
  return false;
}

static void add_default_include_paths(char *argv0)
{
  // We expect that chibicc-specific include files are installed
  // to ./include relative to argv[0].
  strarray_push(&include_paths, format("%s/include", dirname(strdup(argv0))));

  // Add standard include paths.
  strarray_push(&include_paths, "/usr/local/include");
  strarray_push(&include_paths, "/usr/include/x86_64-linux-gnu");
  strarray_push(&include_paths, "/usr/include");
  strarray_push(&include_paths, "/usr/include/chibicc/include");

  // Keep a copy of the standard include paths for -MMD option.
  for (int i = 0; i < include_paths.len; i++)
    strarray_push(&std_include_paths, include_paths.data[i]);
}

static void define(char *str)
{
  char *eq = strchr(str, '=');
  if (eq)
    define_macro(strndup(str, eq - str), eq + 1);
  else
    define_macro(str, "1");
}

static FileType parse_opt_x(char *s)
{
  if (!strcmp(s, "c"))
    return FILE_C;
  if (!strcmp(s, "assembler"))
    return FILE_ASM;
  if (!strcmp(s, "none"))
    return FILE_NONE;
  error("%s : in parse_opt_x <command line>: unknown argument for -x: %s", MAIN_C, s);
}

static char *quote_makefile(char *s)
{
  char *buf = calloc(1, strlen(s) * 2 + 1);
  if (buf == NULL)
  {
    error("%s in quote_makefile buf pointer is null!", MAIN_C);
  }

  for (int i = 0, j = 0; s[i]; i++)
  {
    switch (s[i])
    {
    case '$':
      buf[j++] = '$';
      buf[j++] = '$';
      break;
    case '#':
      buf[j++] = '\\';
      buf[j++] = '#';
      break;
    case ' ':
    case '\t':
      for (int k = i - 1; k >= 0 && s[k] == '\\'; k--)
        buf[j++] = '\\';
      buf[j++] = '\\';
      buf[j++] = s[i];
      break;
    default:
      buf[j++] = s[i];
      break;
    }
  }
  return buf;
}

static void parse_args(int argc, char **argv)
{
  // Make sure that all command line options that take an argument
  // have an argument.
  for (int i = 1; i < argc; i++)
    if (take_arg(argv[i]))
      if (!argv[++i])
      {
        printf("parameter without value! the following parameters need to be followed by a value :\n");
        printf("-o, -I, -idirafter, -include, -x, -MF, -MQ, -MT, -Xlinker, -cc1-input, -cc1-output, -fuse-ld, -soname, -rpath, -z, --version-script \n");
        usage(1);
      }

  StringArray idirafter = {};

  for (int i = 1; i < argc; i++)
  {

    if (!strcmp(argv[i], "-###"))
    {
      opt_hash_hash_hash = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1"))
    {
      opt_cc1 = true;
      continue;
    }

    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
    {
      usage(0);
      continue;
    }

    if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v"))
    {
      printVersion(0);
      continue;
    }

    if (!strcmp(argv[i], "-fuse-ld"))
    {
      opt_linker = argv[++i];
      check_parms_length(opt_linker);
      continue;
    }
    if (!strcmp(argv[i], "-o"))
    {
      opt_o = argv[++i];
      check_parms_length(opt_o);
      continue;
    }

    if (!strncmp(argv[i], "-o", 2))
    {
      opt_o = argv[i] + 2;
      continue;
    }

    if (!strcmp(argv[i], "-S"))
    {
      opt_S = true;
      continue;
    }

    if (!strcmp(argv[i], "-debug"))
    {
      isDebug = true;
      continue;
    }

    if (!strcmp(argv[i], "-dotfile"))
    {
      isDotfile = true;
      continue;
    }

    if (!strcmp(argv[i], "-fcommon"))
    {
      opt_fcommon = true;
      continue;
    }

    if (!strcmp(argv[i], "-fno-common"))
    {
      opt_fcommon = false;
      continue;
    }

    if (!strcmp(argv[i], "-fno-builtin"))
    {
      opt_fbuiltin = false;
      continue;
    }

    if (!strcmp(argv[i], "-c"))
    {
      opt_c = true;
      continue;
    }

    if (!strcmp(argv[i], "-E"))
    {
      opt_E = true;
      continue;
    }

    if (!strncmp(argv[i], "-I", 2))
    {
      strarray_push(&include_paths, argv[i] + 2);
      continue;
    }

    // seems to be used to define macros ?
    if (!strcmp(argv[i], "-D"))
    {
      char *tmp = argv[++i];
      // define(argv[++i]);
      check_parms_length(tmp);
      define(tmp);
      continue;
    }

    if (!strncmp(argv[i], "-D", 2))
    {
      char *tmp = argv[i] + 2;
      check_parms_length(tmp);
      define(tmp);
      // define(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-U"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      undef_macro(tmp);
      // undef_macro(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-U", 2))
    {
      char *tmp = argv[i] + 2;
      check_parms_length(tmp);
      undef_macro(tmp);
      // undef_macro(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-include"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      strarray_push(&opt_include, tmp);
      // strarray_push(&opt_include, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-x"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      opt_x = parse_opt_x(tmp);
      continue;
    }

    if (!strncmp(argv[i], "-x", 2))
    {
      char *tmp = argv[i] + 2;
      check_parms_length(tmp);
      opt_x = parse_opt_x(tmp);
      continue;
    }

    if (!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "-Wl,", 4))
    {
      char *tmp = argv[i];
      check_parms_length(tmp);
      strarray_push(&input_paths, tmp);
      // strarray_push(&input_paths, argv[i]);
      continue;
    }

    if (!strcmp(argv[i], "-Xlinker"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    // not sure what this linker option means
    if (!strcmp(argv[i], "-z"))
    {
      char *tmp = argv[++i];
      printf("%s\n", argv[i]);
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, "-z");
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    // not sure why the --version-script is interpreted as -version-script ?
    if (!strcmp(argv[i], "-version-script"))
    {
      char *tmp = argv[++i];
      printf("%s\n", argv[i]);
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, "--version-script");
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-x"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-s"))
    {
      strarray_push(&ld_extra_args, "-s");
      continue;
    }

    if (!strcmp(argv[i], "-M"))
    {
      opt_M = true;
      continue;
    }

    if (!strcmp(argv[i], "-MF"))
    {
      opt_MF = argv[++i];
      check_parms_length(opt_MF);
      continue;
    }

    if (!strcmp(argv[i], "-MP"))
    {
      opt_MP = true;
      continue;
    }

    if (!strcmp(argv[i], "-MT"))
    {
      if (opt_MT == NULL)
        opt_MT = argv[++i];
      else
        opt_MT = format("%s %s", opt_MT, argv[++i]);
      check_parms_length(opt_MT);
      continue;
    }

    if (!strcmp(argv[i], "-MD"))
    {
      opt_MD = true;
      continue;
    }

    if (!strcmp(argv[i], "-MQ"))
    {
      if (opt_MT == NULL)
        opt_MT = quote_makefile(argv[++i]);
      else
        opt_MT = format("%s %s", opt_MT, quote_makefile(argv[++i]));
      check_parms_length(opt_MT);
      continue;
    }

    if (!strcmp(argv[i], "-MMD"))
    {
      opt_MD = opt_MMD = true;
      continue;
    }

    if (!strcmp(argv[i], "-fpic") || !strcmp(argv[i], "-fPIC"))
    {
      opt_fpic = true;
      continue;
    }

    if (!strcmp(argv[i], "-fno-pic"))
    {
      opt_fpic = false;
      continue;
    }

    if (!strcmp(argv[i], "-cc1-input"))
    {
      base_file = argv[++i];
      check_parms_length(base_file);
      isCc1input = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1-output"))
    {
      isCc1output = true;
      output_file = argv[++i];
      check_parms_length(output_file);
      continue;
    }

    if (!strcmp(argv[i], "-idirafter"))
    {
      char *tmp = argv[i++];
      check_parms_length(tmp);
      strarray_push(&idirafter, tmp);
      // strarray_push(&idirafter, argv[i++]);
      continue;
    }

    if (!strcmp(argv[i], "-static"))
    {
      opt_static = true;
      strarray_push(&ld_extra_args, "-static");
      continue;
    }

    if (!strcmp(argv[i], "-shared"))
    {
      opt_shared = true;
      strarray_push(&ld_extra_args, "-shared");
      continue;
    }

    if (!strcmp(argv[i], "-L"))
    {
      strarray_push(&ld_extra_args, "-L");
      char *tmp = argv[++i];
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-L", 2))
    {
      strarray_push(&ld_extra_args, "-L");
      char *tmp = argv[i] + 2;
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, tmp);
      // strarray_push(&ld_extra_args, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-hashmap-test"))
    {
      hashmap_test();
      exit(0);
    }

    if (!strcmp(argv[i], "-dumpmachine"))
    {
      dump_machine();
      exit(0);
    }

    //-soname create a symbolic link before calling the linker like in gcc -Wl,-soname,libcurl.so.4 -o libcurl.so.4.8.0
    if (!strcmp(argv[i], "-soname"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      strarray_push(&ld_extra_args, "-soname");
      strarray_push(&ld_extra_args, tmp);
      // symbolic_name = tmp;
      continue;
    }

    if (!strcmp(argv[i], "-rpath"))
    {
      char *tmp = argv[++i];
      check_parms_length(tmp);
      r_path = tmp;
      strarray_push(&ld_extra_args, "-rpath");
      strarray_push(&ld_extra_args, r_path);
      continue;
    }

    // These options are ignored for now.
    if (!strncmp(argv[i], "-O", 2) ||
        !strncmp(argv[i], "-W", 2) ||
        !strncmp(argv[i], "-g", 2) ||
        !strncmp(argv[i], "-std=", 5) ||
        !strcmp(argv[i], "-ffreestanding") ||
        !strcmp(argv[i], "-fno-omit-frame-pointer") ||
        !strcmp(argv[i], "-fno-stack-protector") ||
        !strcmp(argv[i], "-fno-strict-aliasing") ||
        !strcmp(argv[i], "-m64") ||
        !strcmp(argv[i], "-m32") ||
        !strcmp(argv[i], "-pthread") ||
        !strcmp(argv[i], "-pedantic") ||
        !strcmp(argv[i], "-nostdinc") ||
        !strcmp(argv[i], "-mno-red-zone") ||
        !strcmp(argv[i], "-w"))
      continue;

    if (argv[i][0] == '-' && argv[i][1] != '\0')
      error("%s in parse_args unknown argument: %s", MAIN_C, argv[i]);

    strarray_push(&input_paths, argv[i]);
  }

  for (int i = 0; i < idirafter.len; i++)
    strarray_push(&include_paths, idirafter.data[i]);

  if (input_paths.len == 0)
    error("%s : in parse_args no input files", MAIN_C);

  // -E implies that the input is the C macro language.
  if (opt_E)
    opt_x = FILE_C;
}

static FILE *open_file(char *path)
{
  if (!path || strcmp(path, "-") == 0)
    return stdout;

  FILE *out = fopen(path, "w");
  if (!out)
    error("%s : in open_file cannot open output file: %s: %s", MAIN_C, path, strerror(errno));
  return out;
}

static bool endswith(char *p, char *q)
{
  int len1 = strlen(p);
  int len2 = strlen(q);
  return (len1 >= len2) && !strcmp(p + len1 - len2, q);
}

// returns filename example ./test/hello.c returns hello.c
char *extract_filename(char *tmpl)
{
  char *filename = basename(strdup(tmpl));
  return format("%s", filename);
}

// return path without filename example : ./test/hello.c returns ./test
char *extract_path(char *tmpl, char *basename)
{
  int total_length = strlen(tmpl);
  int basename_length = strlen(basename);
  int length = total_length - basename_length + 1;
  char pathonly[length];
  memset(pathonly, 0, sizeof(pathonly));
  strncpy(pathonly, tmpl, sizeof(pathonly) - 1);
  return format("%s", pathonly);
}

// Replace file extension
char *replace_extn(char *tmpl, char *extn)
{
  char *filename = extract_filename(tmpl);
  // char *filename = basename(strdup(tmpl));
  char *dot = strrchr(filename, '.');
  if (dot)
    *dot = '\0';
  return format("%s%s", filename, extn);
}

static void cleanup(void)
{
  if (!isDebug)
  {
    for (int i = 0; i < tmpfiles.len; i++)
      unlink(tmpfiles.data[i]);
  }

  if (isDebug && f != NULL)
    fclose(f);

  // for dot diagrams
  if (isDotfile && dotf != NULL)
  {
    fprintf(dotf, "}\n");
    fclose(dotf);
  }
}

static char *create_tmpfile(void)
{
  char *path = strdup("/tmp/chibicc-XXXXXX");
  if (path == NULL)
    error("%s : in create_tmpfile path path is null", MAIN_C);
  int fd = mkstemp(path);
  if (fd == -1)
    error("%s : in create_tmpfile mkstemp failed: %s", MAIN_C, strerror(errno));
  close(fd);

  strarray_push(&tmpfiles, path);
  return path;
}

static void run_subprocess(char **argv)
{
  // If -### or -debug is given, dump the subprocess's command line.
  if (opt_hash_hash_hash)
  {
    fprintf(stdout, "%s", argv[0]);
    for (int i = 1; argv[i]; i++)
      fprintf(stdout, " %s", argv[i]);
    fprintf(stdout, "\n");
  }

  if (isDebug && f != NULL)
  {
    fprintf(f, "%s", argv[0]);
    for (int i = 1; argv[i]; i++)
      fprintf(f, " %s", argv[i]);
    fprintf(f, "\n");
  }

  if (fork() == 0)
  {
    // sanitize environment variables here only PATH and IFS.
    spc_sanitize_environment();

    execvp(argv[0], argv);
    fprintf(stderr, "%s : in run_subprocess exec failed: %s: %s\n", MAIN_C, argv[0], strerror(errno));
    _exit(99);
  }

  // Wait for the child process to finish.
  int status;
  while (wait(&status) > 0)
    ;
  if (status != 0)
    exit(1);
}

static void run_cc1(int argc, char **argv, char *input, char *output)
{
  char **args = calloc(argc + 10, sizeof(char *));
  if (args == NULL)
    error("%s : in run_cc1 args is null", MAIN_C);
  memcpy(args, argv, argc * sizeof(char *));
  args[argc++] = "-cc1";

  if (input)
  {
    args[argc++] = "-cc1-input";
    args[argc++] = input;
  }

  if (output)
  {
    args[argc++] = "-cc1-output";
    args[argc++] = output;
  }
  // only to compile VLC if not it failed without these definitions set up
  run_subprocess(args);
  free(args);
}

// Print tokens to stdout. Used for -E.
static void print_tokens(Token *tok)
{
  FILE *out = open_file(opt_o ? opt_o : "-");

  int line = 1;
  for (; tok->kind != TK_EOF; tok = tok->next)
  {
    if (line > 1 && tok->at_bol)
      fprintf(out, "\n");
    if (tok->has_space && !tok->at_bol)
      fprintf(out, " ");
    fprintf(out, "%.*s", tok->len, tok->loc);
    line++;
  }
  fprintf(out, "\n");
}

static bool in_std_include_path(char *path)
{
  for (int i = 0; i < std_include_paths.len; i++)
  {
    char *dir = std_include_paths.data[i];
    int len = strlen(dir);
    if (strncmp(dir, path, len) == 0 && path[len] == '/')
      return true;
  }
  return false;
}

// If -M options is given, the compiler write a list of input files to
// stdout in a format that "make" command can read. This feature is
// used to automate file dependency management.
static void print_dependencies(void)
{
  char *path;
  if (opt_MF)
    path = opt_MF;
  else if (opt_MD)
  {
    // fixing the issue with file.d created in the current directory if no -o parameter or in the output directory define by -o parameter
    path = replace_extn(opt_o ? opt_o : base_file, ".d");
    if (opt_o != NULL)
    {
      char *fullpath;
      char *filename;
      filename = extract_filename(opt_o);
      fullpath = extract_path(opt_o, filename);
      strncat(fullpath, path, strlen(path));
      path = fullpath;
    }
  }
  else if (opt_o)
    path = opt_o;
  else
    path = "-";

  FILE *out = open_file(path);
  if (opt_MT)
    fprintf(out, "%s:", opt_MT);
  else
    fprintf(out, "%s:", quote_makefile(replace_extn(base_file, ".o")));

  File **files = get_input_files();

  for (int i = 0; files[i]; i++)
  {
    if (opt_MMD && in_std_include_path(files[i]->name))
      continue;
    fprintf(out, " \\\n  %s", files[i]->name);
  }

  fprintf(out, "\n\n");

  if (opt_MP)
  {
    for (int i = 1; files[i]; i++)
    {
      if (opt_MMD && in_std_include_path(files[i]->name))
        continue;
      fprintf(out, "%s:\n\n", quote_makefile(files[i]->name));
    }
  }
}

static Token *must_tokenize_file(char *path)
{
  Token *tok = tokenize_file(path);
  if (!tok)
    error("%s : in must_tokenize_file %s: %s", MAIN_C, path, strerror(errno));
  return tok;
}

static Token *append_tokens(Token *tok1, Token *tok2)
{
  if (!tok1 || tok1->kind == TK_EOF)
    return tok2;

  Token *t = tok1;
  while (t->next->kind != TK_EOF)
    t = t->next;
  t->next = tok2;
  return tok1;
}

static void cc1(void)
{
  Token *tok = NULL;

  // Process -include option
  for (int i = 0; i < opt_include.len; i++)
  {
    char *incl = opt_include.data[i];

    char *path;
    if (file_exists(incl))
    {
      path = incl;
    }
    else
    {
      path = search_include_paths(incl);
      if (!path)
        error("%s : in cc1 -include: %s: %s", MAIN_C, incl, strerror(errno));
    }

    Token *tok2 = must_tokenize_file(path);
    tok = append_tokens(tok, tok2);
  }

  // Tokenize and parse.
  Token *tok2 = must_tokenize_file(base_file);
  tok = append_tokens(tok, tok2);
  tok = preprocess(tok);

  // If -M or -MD are given, print file dependencies.
  if (opt_M || opt_MD)
  {
    print_dependencies();
    if (opt_M)
      return;
  }

  // If -E is given, print out preprocessed C code as a result.
  if (opt_E)
  {
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

static void assemble(char *input, char *output)
{
  char *cmd[] = {"as", "-c", input, "-o", output, NULL};
  run_subprocess(cmd);
}

// static void symbolic_link(char *input, char *output) {
//   char *cmd[] = {"ln", "-s", "-f", output, input, NULL};
//   run_subprocess(cmd);
// }

void dump_machine(void)
{
  fprintf(stdout, DEFAULT_TARGET_MACHINE "\n");
}

static char *find_file(char *pattern)
{
  char *path = NULL;
  glob_t buf = {};
  glob(pattern, 0, NULL, &buf);
  if (buf.gl_pathc > 0)
    path = strdup(buf.gl_pathv[buf.gl_pathc - 1]);
  globfree(&buf);
  return path;
}

// Returns true if a given file exists.
bool file_exists(char *path)
{
  struct stat st;
  return !stat(path, &st);
}

static char *find_libpath(void)
{
  if (file_exists("/usr/lib/x86_64-linux-gnu/crti.o"))
    return "/usr/lib/x86_64-linux-gnu";
  if (file_exists("/usr/lib64/crti.o"))
    return "/usr/lib64";
  error("%s : in find_libpath library path is not found", MAIN_C);
}

static char *find_gcc_libpath(void)
{
  char *paths[] = {
      "/usr/lib/gcc/x86_64-linux-gnu/*/crtbegin.o",
      "/usr/lib/gcc/x86_64-*/*/crtbegin.o",
      "/usr/lib/gcc/x86_64-pc-linux-gnu/*/crtbegin.o", // For Gentoo
      "/usr/lib/gcc/x86_64-redhat-linux/*/crtbegin.o", // For Fedora
  };

  for (int i = 0; i < sizeof(paths) / sizeof(*paths); i++)
  {
    char *path = find_file(paths[i]);
    if (path)
      return dirname(path);
  }

  error("%s : in find_gcc_libpath gcc library path is not found", MAIN_C);
}

static void run_linker(StringArray *inputs, char *output)
{
  StringArray arr = {};
  if (opt_linker != NULL)
  {
    strarray_push(&arr, opt_linker);
  }
  else
  {
    strarray_push(&arr, "ld");
  }
  strarray_push(&arr, "-o");
  strarray_push(&arr, output);
  strarray_push(&arr, "-m");
  strarray_push(&arr, "elf_x86_64");

  char *libpath = find_libpath();
  char *gcc_libpath = find_gcc_libpath();

  if (opt_shared)
  {
    strarray_push(&arr, format("%s/crti.o", libpath));
    strarray_push(&arr, format("%s/crtbeginS.o", gcc_libpath));
  }
  else
  {
    strarray_push(&arr, format("%s/crt1.o", libpath));
    strarray_push(&arr, format("%s/crti.o", libpath));
    strarray_push(&arr, format("%s/crtbegin.o", gcc_libpath));
  }

  strarray_push(&arr, format("-L%s", gcc_libpath));
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");
  strarray_push(&arr, "-L/usr/lib64");
  strarray_push(&arr, "-L/lib64");
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");
  strarray_push(&arr, "-L/usr/lib/x86_64-pc-linux-gnu");
  strarray_push(&arr, "-L/usr/lib/x86_64-redhat-linux");
  strarray_push(&arr, "-L/usr/lib");
  strarray_push(&arr, "-L/lib");

  if (!opt_static)
  {
    strarray_push(&arr, "-dynamic-linker");
    strarray_push(&arr, "/lib64/ld-linux-x86-64.so.2");
  }

  for (int i = 0; i < ld_extra_args.len; i++)
    strarray_push(&arr, ld_extra_args.data[i]);

  for (int i = 0; i < inputs->len; i++)
    strarray_push(&arr, inputs->data[i]);

  if (opt_static)
  {
    strarray_push(&arr, "--start-group");
    strarray_push(&arr, "-lgcc");
    strarray_push(&arr, "-lgcc_eh");
    strarray_push(&arr, "-lc");
    strarray_push(&arr, "--end-group");
  }
  else
  {
    strarray_push(&arr, "-lc");
    strarray_push(&arr, "-lgcc");
    strarray_push(&arr, "--as-needed");
    strarray_push(&arr, "-lgcc_s");
    strarray_push(&arr, "--no-as-needed");
  }

  if (opt_shared)
    strarray_push(&arr, format("%s/crtendS.o", gcc_libpath));
  else
    strarray_push(&arr, format("%s/crtend.o", gcc_libpath));

  strarray_push(&arr, format("%s/crtn.o", libpath));
  strarray_push(&arr, NULL);

  run_subprocess(arr.data);
}

static FileType get_file_type(char *filename)
{
  if (opt_x != FILE_NONE)
    return opt_x;

  if (endswith(filename, ".a"))
    return FILE_AR;
  if (endswith(filename, ".so"))
    return FILE_DSO;
  if (endswith(filename, ".lo"))
    return FILE_DSO;
  if (endswith(filename, ".o"))
    return FILE_OBJ;
  if (endswith(filename, ".c"))
    return FILE_C;
  if (endswith(filename, ".s"))
    return FILE_ASM;
  if (endswith(filename, ".so.4"))
    return FILE_DSO;

  error("%s : in get_file_type <command line>: unknown file extension: %s", MAIN_C, filename);
}

int main(int argc, char **argv)
{
  atexit(cleanup);

  if (isDebug)
  {
    f = fopen(logFile, "w");
    if (f == NULL)
    {
      error("%s : in main Issue with -debug parameter, file not opened!", MAIN_C);
      exit(2);
    }
  }

  init_macros();

  int isInvalidArg = validateArgs(argc, argv);
  if (isInvalidArg == -1)
  {
    error("%s : in main Invalid parameter detected!", MAIN_C);
    usage(-2);
  }
  parse_args(argc, argv);

  if (opt_cc1 && !isCc1input)
  {
    error("%s : in main with -cc1 parameter -cc1-input is mandatory!", MAIN_C);
    usage(-1);
  }

  if (opt_cc1)
  {
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }

  if (input_paths.len > 1 && opt_o && (opt_c || opt_S || opt_E))
    error("%s : in main cannot specify '-o' with '-c,' '-S' or '-E' with multiple files", MAIN_C);

  StringArray ld_args = {};

  for (int i = 0; i < input_paths.len; i++)
  {
    char *input = input_paths.data[i];

    if (!strncmp(input, "-l", 2))
    {
      strarray_push(&ld_args, input);
      continue;
    }

    if (!strncmp(input, "-Wl,", 4))
    {
      char *s = strdup(input + 4);
      char *arg = strtok(s, ",");
      while (arg)
      {
        strarray_push(&ld_args, arg);
        arg = strtok(NULL, ",");
      }
      continue;
    }

    char *output;
    if (opt_o)
      output = opt_o;
    else if (opt_S)
      output = replace_extn(input, ".s");
    else
      output = replace_extn(input, ".o");

    FileType type = get_file_type(input);

    // Handle .o or .a
    if (type == FILE_OBJ || type == FILE_AR || type == FILE_DSO)
    {
      strarray_push(&ld_args, input);
      continue;
    }

    // Handle .s
    if (type == FILE_ASM)
    {
      if (!opt_S)
        assemble(input, output);
      continue;
    }

    assert(type == FILE_C);

    // Just preprocess
    if (opt_E || opt_M)
    {
      run_cc1(argc, argv, input, NULL);
      continue;
    }

    // Compile
    if (opt_S)
    {
      run_cc1(argc, argv, input, output);
      continue;
    }

    // Compile and assemble
    if (opt_c)
    {
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
  {
    // if (symbolic_name)
    //   symbolic_link(symbolic_name, opt_o);
    run_linker(&ld_args, opt_o ? opt_o : "a.out");
  }

  free(opt_MT);

  return 0;
}
