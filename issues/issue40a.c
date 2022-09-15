// fails silently (pointer to VLA)
int x = 10;
void fun(char (*a)[x]) { sizeof *a; }


int main(void) {
    return 0;
}