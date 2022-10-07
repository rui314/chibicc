#include <stdio.h>
#define SIZE 10

extern int myarr[SIZE];
int myarr[] = {11, 34, };
int main(void) {
    for(int i = 0; i < SIZE; i++) {
        printf("%d: %d\n", i, myarr[i]);

    }
    return 0;
}