#include <stddef.h>
#define NID_sha1 64
#define NID_sha224 675
#define NID_sha256 672
#define NID_sha384 673
#define NID_sha512 674
#define NID_sha512_224 1094
#define NID_sha512_256 1095

static const unsigned char der_aid_mgf1SHA512_224Identifier[] = {

};

static const unsigned char der_aid_mgf1SHA512Identifier[] = {

};

static const unsigned char der_aid_mgf1SHA384Identifier[] = {

};
static const unsigned char der_aid_mgf1SHA256Identifier[] = {

};

static const unsigned char der_aid_mgf1SHA224Identifier[] = {

};

static const unsigned char der_aid_mgf1SHA512_256Identifier[] = {

};

#define MGF1_SHA_CASE(bits, var)                              \
    case NID_sha##bits:                                       \
        var = der_aid_mgf1SHA##bits##Identifier;              \
        var##_sz = sizeof(der_aid_mgf1SHA##bits##Identifier); \
        break;

int main(void)
{
    int maskgenhashalg_nid = 0;
    const unsigned char *maskgenalg = NULL;
    size_t maskgenalg_sz = 0;

    switch (maskgenhashalg_nid)
    {
    case NID_sha1:
        break;
        // MGF1_SHA_CASE(224, maskgenalg);
        // MGF1_SHA_CASE(256, maskgenalg);
        // MGF1_SHA_CASE(384, maskgenalg);
        // MGF1_SHA_CASE(512, maskgenalg);
        MGF1_SHA_CASE(512_224, maskgenalg);
        MGF1_SHA_CASE(512_256, maskgenalg);
    default:
        return 0;
    }

    return 0;
}