#ifndef __STDFLOAT_H
#define __STDFLOAT_H

#define DECIMAL_DIG 21
#define FLT_EVAL_METHOD 0 // C11 5.2.4.2.2p9
#define FLT_RADIX 2
#define FLT_ROUNDS 1      // C11 5.2.4.2.2p8: to nearest

#define FLT_DIG 6
#define FLT_EPSILON 0x1p-23
#define FLT_MANT_DIG 24
#define FLT_MAX 0x1.fffffep+127
#define FLT_MAX_10_EXP 38
#define FLT_MAX_EXP 128
#define FLT_MIN 0x1p-126
#define FLT_MIN_10_EXP -37
#define FLT_MIN_EXP -125
#define FLT_TRUE_MIN 0x1p-149

#define DBL_DIG 15
#define DBL_EPSILON 0x1p-52
#define DBL_MANT_DIG 53
#define DBL_MAX 0x1.fffffffffffffp+1023
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define DBL_MIN 0x1p-1022
#define DBL_MIN_10_EXP -307
#define DBL_MIN_EXP -1021
#define DBL_TRUE_MIN 0x0.0000000000001p-1022

#define LDBL_DIG 15
#define LDBL_EPSILON 0x1p-52
#define LDBL_MANT_DIG 53
#define LDBL_MAX 0x1.fffffffffffffp+1023
#define LDBL_MAX_10_EXP 308
#define LDBL_MAX_EXP 1024
#define LDBL_MIN 0x1p-1022
#define LDBL_MIN_10_EXP -307
#define LDBL_MIN_EXP -1021
#define LDBL_TRUE_MIN 0x0.0000000000001p-1022

#endif
