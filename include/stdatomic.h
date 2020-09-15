#ifndef __STDATOMIC_H
#define __STDATOMIC_H

#define atomic_compare_exchange_weak(p, old, new) \
  __builtin_compare_and_swap((p), (old), (new))

#define atomic_compare_exchange_strong(p, old, new) \
  __builtin_compare_and_swap((p), (old), (new))

#endif
