#ifndef VEC_ALLOC_H
#define VEC_ALLOC_H

#include "prelude.h"

#define DEFAULT_SIZE 16

void* alloc(usize size);
void _vec_grow_amortized(Vec* self,usize additional);


#endif