#!/usr/bin/python3
import re
import sys

print("""
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef unsigned long size_t;

typedef struct FILE FILE;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

typedef struct {
  int gp_offset;
  int fp_offset;
  void *overflow_arg_area;
  void *reg_save_area;
} __va_elem;

typedef __va_elem va_list[1];

struct stat {
  char _[512];
};

void *malloc(long size);
void *calloc(long nmemb, long size);
void *realloc(void *buf, long size);
int *__errno_location();
char *strerror(int errnum);
FILE *fopen(char *pathname, char *mode);
long fread(void *ptr, long size, long nmemb, FILE *stream);
int fclose(FILE *fp);
int feof(FILE *stream);
static void assert() {}
int strcmp(char *s1, char *s2);
int strncasecmp(char *s1, char *s2, long n);
int printf(char *fmt, ...);
int sprintf(char *buf, char *fmt, ...);
int fprintf(FILE *fp, char *fmt, ...);
int vfprintf(FILE *fp, char *fmt, va_list ap);
long strlen(char *p);
int strncmp(char *p, char *q, long n);
void *memcpy(char *dst, char *src, long n);
char *strdup(char *p);
char *strndup(char *p, long n);
int isspace(int c);
int ispunct(int c);
int isdigit(int c);
int isxdigit(int c);
char *strstr(char *haystack, char *needle);
char *strchr(char *s, int c);
double strtod(char *nptr, char **endptr);
static void va_end(va_list ap) {}
long strtoul(char *nptr, char **endptr, int base);
void exit(int code);
char *basename(char *path);
char *strrchr(char *s, int c);
int unlink(char *pathname);
int mkstemp(char *template);
int close(int fd);
int fork(void);
int execvp(char *file, char **argv);
void _exit(int code);
int wait(int *wstatus);
int atexit(void (*)(void));
FILE *open_memstream(char **ptr, size_t *sizeloc);
char *dirname(char *path);
char *strncpy(char *dest, char *src, long n);
""")

for path in sys.argv[1:]:
    with open(path) as file:
        s = file.read()
        s = re.sub(r'\\\n', '', s)
        s = re.sub(r'^\s*#.*', '', s, flags=re.MULTILINE)
        s = re.sub(r'"\n\s*"', '', s)
        s = re.sub(r'\bbool\b', '_Bool', s)
        s = re.sub(r'\berrno\b', '*__errno_location()', s)
        s = re.sub(r'\btrue\b', '1', s)
        s = re.sub(r'\bfalse\b', '0', s)
        s = re.sub(r'\bNULL\b', '0', s)
        s = re.sub(r'\bva_start\(([^)]*),([^)]*)\)', '*(\\1)=*(__va_elem*)__va_area__', s)
        s = re.sub(r'\bunreachable\(\)', 'error("unreachable")', s)
        s = re.sub(r'\bMIN\(([^)]*),([^)]*)\)', '((\\1)<(\\2)?(\\1):(\\2))', s)
        print(s)
