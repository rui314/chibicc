#include <stdio.h>
long double x = 0;

int main(void) {
    printf("%Lf %ld\n", x, sizeof(x));
    return 0;
}