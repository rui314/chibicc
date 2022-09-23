int main(void) {
    int a = 10, b = 0;
   asm ("movl %eax, %ebx\n\t" 
    "movl $56, %esi\n\t"
    "movb %ah, (%ebx)");
    return 0;
}