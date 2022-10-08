#include <stddef.h>
struct DH
{
    char *field1;
    char *field2;
};
typedef struct DH DH;

#define make_dh(x)         \
    char *DH_get_##x(void) \
    {                      \
        char *dh;          \
        if (dh == NULL)    \
            return NULL;   \
        return dh;         \
    }

char *DH_get_tt1024_160()
{
    char *dh;
    return dh;
}

int main(void)
{

    make_dh(1024_160);
    return 0;
}
