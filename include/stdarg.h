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

static void *__va_arg_gp(__va_elem *ap) {
  void *r = (char *)ap->reg_save_area + ap->gp_offset;
  ap->gp_offset += 8;
  return r;
}

static void *__va_arg_fp(__va_elem *ap) {
  void *r = (char *)ap->reg_save_area + ap->fp_offset;
  ap->fp_offset += 8;
  return r;
}

static void *__va_arg_mem(__va_elem *ap) {
  1 / 0; // not implemented
}

#define va_arg(ap, type)                        \
  ({                                            \
    int klass = __builtin_reg_class(type);      \
    *(type *)(klass == 0 ? __va_arg_gp(ap) :    \
              klass == 1 ? __va_arg_fp(ap) :    \
              __va_arg_mem(ap));                \
  })

#define __GNUC_VA_LIST 1
typedef va_list __gnuc_va_list;

#endif
