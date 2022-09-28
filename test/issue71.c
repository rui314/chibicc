_Atomic long double b;

void bug(void) {
    b += 3;
}

int main(void) {
    return 0;
}