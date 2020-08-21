#define ASSERT(x, y) assert(x, y, #y)

int assert(int expected, int actual, char *code);
int printf();
