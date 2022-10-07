// a.c
struct S { int n; };

int main(void) {
    return (struct S){1}.n;
}