#ifndef VEC_ALLOC_H
#define VEC_ALLOC_H

#include "prelude.h"

#ifdef _cplusplus
extern "C" {
#endif

static inline void* __alloc(usize size);
static bool _vec_grow_amortized(Vec* self,usize additional);
static void _drop_in_place(void* ptr,usize count,usize BYTES_PER_ELEMENT,Destructor destructor);

#ifdef _cplusplus
}
#endif
#endif
