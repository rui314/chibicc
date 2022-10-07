#define OSSL_CORE_MAKE_FUNC(type,name,args)                             \
    typedef type (OSSL_FUNC_##name##_fn)args;                           \
    static ossl_unused ossl_inline \
    OSSL_FUNC_##name##_fn *OSSL_FUNC_##name(const OSSL_DISPATCH *opf)   \
    {                                                                   \
        return (OSSL_FUNC_##name##_fn *)opf->function;                  \
    }


OSSL_CORE_MAKE_FUNC(int, decoder_decode, (int selection))

static OSSL_FUNC_decoder_decode_fn der2obj_decode;
static int der2obj_decode(int selection)
{
    MGF1_SHA_CASE(512_256, maskgenalg);    
    return 1;
}

int main(void) {
    return 0;
}