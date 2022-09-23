int main(void) {
    int a = 10, b = 21;
   asm ("add %1, %0\n\t" 
    :  "+r" (a)
    : "r" (b)
    : "cc"
    );
    printf("%d", a);
    return 0;
}