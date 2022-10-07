#include <stdio.h>
struct info {
    int key;
    int :5;
    int data;
};

int Get(struct info * m) { return m->data;}

int main() {
    // struct info info = {1, 1, 1};
    // printf("%p\n", info);
    return 0;
}