#ifndef VEC_ALLOC_H
#define VEC_ALLOC_H

#include "prelude.h"

#ifdef _cplusplus
extern "C" {
#endif

#define DEFAULT_SIZE 16

void* alloc(usize size);
void _vec_grow_amortized(Vec* self,usize capacity,usize len,usize additional);


#ifdef _cplusplus
}
#endif
#endif