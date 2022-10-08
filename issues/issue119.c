
#if !defined(__DJGPP__)
#if defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 199901L
#include <complex.h>
#endif
#endif
#endif
#include <stdlib.h>

// chibicc  -Iinclude -Iapps/include  -pthread -m64 -Wall -O3 -DOPENSSL_BUILDING_OPENSSL -DNDEBUG   -c -o test/rsa_complex-bin-rsa_complex.o test/rsa_complex.c
//  /usr/include/x86_64-linux-gnu/bits/cmathcalls.h:55: __MATHCALL (cacos, (_Mdouble_complex_ __z));
//                                                                                           ^ expected ','

int main(void)
{
    return 0;
}