#!/bin/bash -x
TMP=tmp-self

mkdir -p $TMP

expand() {
    file=$1
    cat <<EOF > $TMP/$1
typedef struct FILE FILE;
extern FILE *stdout;
extern FILE *stderr;

void *malloc(long size);
void *calloc(long nmemb, long size);
int *__errno_location();
char *strerror(int errnum);
FILE *fopen(char *pathname, char *mode);
long fread(void *ptr, long size, long nmemb, FILE *stream);
int feof(FILE *stream);
static void assert() {}
int strcmp(char *s1, char *s2);
int printf(char *fmt, ...);
int sprintf(char *buf, char *fmt, ...);
long strlen(char *p);
int strncmp(char *p, char *q);
void *memcpy(char *dst, char *src, long n);
char *strndup(char *p, long n);
int isspace(int c);
char *strstr(char *haystack, char *needle);                                              
long strtol(char *nptr, char **endptr, int base);
EOF

    grep -v '^#' chibi.h >> $TMP/$1
    grep -v '^#' $1 >> $TMP/$1
    sed -i 's/\bbool\b/_Bool/g' $TMP/$1
    sed -i 's/\berrno\b/*__errno_location()/g' $TMP/$1
    sed -i 's/\btrue\b/1/g; s/\bfalse\b/0/g;' $TMP/$1
    sed -i 's/\bNULL\b/0/g' $TMP/$1
    sed -i 's/, \.\.\.//g' $TMP/$1
    sed -i 's/INT_MAX/2147483647/g' $TMP/$1

    ./chibicc $TMP/$1 > $TMP/${1%.c}.s
    gcc -c -o $TMP/${1%.c}.o $TMP/${1%.c}.s
}

cp *.c $TMP
for i in $TMP/*.c; do
  gcc -I. -c -o ${i%.c}.o $i
done

expand main.c
expand type.c
expand parse.c
expand codegen.c

gcc -static -o chibicc-gen2 $TMP/*.o
