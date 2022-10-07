
#ifndef VLC_COMMON_H
# define VLC_COMMON_H 1

// #if defined(__gnu__) && __has_attribute
// #define test4 "test4"
// #endif

#if defined(__clang__) && __has_attribute(diagnose_if)
#define test4 "test4"
#endif

#endif /* !VLC_COMMON_H */
