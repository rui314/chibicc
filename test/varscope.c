#include <stdio.h>


void Increment(int a) {

    a = a + 1;
    printf("Address of a inside the function %d\n", &a );
}
int main(int argc, char **argv) {
int a = 10;
Increment(a);
printf("Address of a inside the function %d\n", &a );
return 0;
}