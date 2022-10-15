
/* Function attributes for compiler warnings */
#if defined __has_attribute
#if __has_attribute(warning)
#define VLC_WARN_CALL(w) VLC_NOINLINE_FUNC __attribute__((warning((w))))
#else
#define VLC_WARN_CALL(w)
#endif

#if __has_attribute(error)
#define VLC_ERROR_CALL(e) VLC_NOINLINE_FUNC __attribute__((error((e))))
#else
#define VLC_ERROR_CALL(e)
#endif

#if __has_attribute(unused)
#define VLC_UNUSED_FUNC __attribute__((unused))
#else
#define VLC_UNUSED_FUNC
#endif

#if __has_attribute(noinline)
#define VLC_NOINLINE_FUNC __attribute__((noinline))
#else
#define VLC_NOINLINE_FUNC
#endif

#if __has_attribute(deprecated)
#define VLC_DEPRECATED __attribute__((deprecated))
#else
#define VLC_DEPRECATED
#endif

#if __has_attribute(malloc)
#define VLC_MALLOC __attribute__((malloc))
#else
#define VLC_MALLOC
#endif

#if __has_attribute(warn_unused_result)
#define VLC_USED __attribute__((warn_unused_result))
#else
#define VLC_USED
#endif
#else

#endif
// NOTHING int myfunc2(int x) {
//     return x;
// }

VLC_USED int myfunc(int x)
{
    return x;
}

int main(void)
{

    return 0;
}
