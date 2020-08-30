#ifndef __STDARG_H
#define __STDARG_H

typedef struct {
  unsigned int gp_offset;
  unsigned int fp_offset;
  void *overflow_arg_area;
  void *reg_save_area;
} __va_elem;

typedef __va_elem va_list[1];

#define va_start(ap, last) \
  do { *(ap) = *(__va_elem *)__va_area__; } while (0)

#define va_end(ap)

#define __GNUC_VA_LIST 1
typedef va_list __gnuc_va_list;

#endif
