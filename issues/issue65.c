struct S { unsigned long long int b:32; } s;

void f() {
    s.b = 0;
}
int main() {
    return 0;
}