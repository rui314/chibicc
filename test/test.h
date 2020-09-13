#define ASSERT(x, y) assert(x, y, #y)

int assert(int expected, int actual, char *code);
int printf();
int sprintf();
int strcmp(char *p, char *q);
int memcmp(char *p, char *q, long n);
