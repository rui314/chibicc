#include <stddef.h>

#define OSSL_FUNC_DECODER_NEWCTX 1
#define OSSL_FUNC_DECODER_FREECTX 2
#define OSSL_FUNC_DECODER_GET_PARAMS 3
#define OSSL_FUNC_DECODER_GETTABLE_PARAMS 4
#define OSSL_FUNC_DECODER_SET_CTX_PARAMS 5
#define OSSL_FUNC_DECODER_SETTABLE_CTX_PARAMS 6
#define OSSL_FUNC_DECODER_DOES_SELECTION 10
#define OSSL_FUNC_DECODER_DECODE 11
#define OSSL_FUNC_DECODER_EXPORT_OBJECT 20

#define OSSL_OBJECT_UNKNOWN 0
#define OSSL_OBJECT_NAME 1 /* char * */
#define OSSL_OBJECT_PKEY 2 /* EVP_PKEY * */
#define OSSL_OBJECT_CERT 3 /* X509 * */
#define OSSL_OBJECT_CRL 4  /* X509_CRL * */

struct ossl_dispatch_st
{
    int function_id;
    void (*function)(void);
};

static void any2obj_freectx(void *vctx)
{
}

static void *any2obj_newctx(void *provctx)
{
    return provctx;
}

typedef struct ossl_dispatch_st OSSL_DISPATCH;

static void *pem2der_newctx(void *provctx)
{
    return provctx;
}

static void pem2der_freectx(void *vctx)
{
    void *tmp = vctx;
}

static int der2obj_decode(void *provctx)
{
    return 0;
}

static int pvk2obj_decode(void *provctx)
{
    return 0;
}

static int msblob2obj_decode(void *provctx)
{
    return 0;
}

static int any2obj_decode_final(void *provctx)
{
    return 0;
}

static int pem2der_decode(void *vctx)
{

#define MAKE_DECODER(fromtype, objtype)                                    \
    static const OSSL_DISPATCH fromtype##_to_obj_decoder_functions[] = {   \
        {OSSL_FUNC_DECODER_NEWCTX, (void (*)(void))any2obj_newctx},        \
        {OSSL_FUNC_DECODER_FREECTX, (void (*)(void))any2obj_freectx},      \
        {OSSL_FUNC_DECODER_DECODE, (void (*)(void))fromtype##2obj_decode}, \
        {0, NULL}                                                          \
    }

    MAKE_DECODER(der, OSSL_OBJECT_UNKNOWN);
    MAKE_DECODER(msblob, OSSL_OBJECT_PKEY);
    MAKE_DECODER(pvk, OSSL_OBJECT_PKEY);

    return 0;
}
const OSSL_DISPATCH ossl_pem_to_der_decoder_functions[] = {
    {OSSL_FUNC_DECODER_NEWCTX, (void (*)(void))pem2der_newctx},
    {OSSL_FUNC_DECODER_FREECTX, (void (*)(void))pem2der_freectx},
    {OSSL_FUNC_DECODER_DECODE, (void (*)(void))pem2der_decode},
    {0, NULL}};

int main(void)
{
    return 0;
}
