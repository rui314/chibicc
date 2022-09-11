typedef struct empty_struct {} empty_struct;

int f(empty_struct a) {
    return 0;
}

int main() {
    f((empty_struct){});
    return 0;
}