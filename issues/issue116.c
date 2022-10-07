struct DH {
    char * field1;
    char * field2;
};
typedef struct DH DH;

#define make_dh(x) DH *DH_get_##x(void) { char *dh; return dh; }
int main(void) {
    return 0;
}
