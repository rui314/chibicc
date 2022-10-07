struct S {
    union {
        int A;

    };
};

int main(void) {
    (struct S){.A = 0};
    return 0;
}