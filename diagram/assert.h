#ifndef	_ASSERT_H
#define	_ASSERT_H
# define __ASSERT_VOID_CAST (void)

/* void assert (int expression);

   If NDEBUG is defined, do nothing.
   If not, and EXPRESSION is zero, print an error message and abort.  */

#  define assert(expr)							\
    ((expr)								\
     ? __ASSERT_VOID_CAST (0)						\
     : __assert_fail (#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))

#  define assert_perror(errnum)						\
  (!(errnum)								\
   ? __ASSERT_VOID_CAST (0)						\
   : __assert_perror_fail ((errnum), __FILE__, __LINE__, __ASSERT_FUNCTION))


# define static_assert _Static_assert

# define _Static_assert(expr, diagnostic) \
    extern int (*__Static_assert_function (void)) \
      [!!sizeof (struct { int __error_if_negative: (expr) ? 2 : -1; })]

#endif