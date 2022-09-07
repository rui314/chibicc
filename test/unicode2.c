#include <fcntl.h>
#include <stdio.h>

int main(void) {
    printf("\xE2\x98\xA0 \n");
    printf("\U0001F602\n");
    printf("\u2620\n");
    printf("\x09\x65\n");
    printf("\u0965\n");
    printf("%d\n", '\\');
    return 0;
}