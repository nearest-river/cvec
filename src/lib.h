#ifndef VEC_LIB_H
#define VEC_LIB_H

#include "prelude.h"


typedef __SIZE_TYPE__ usize;

typedef struct Vec {
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
} Vec;


void* new_vec(usize BYTES_PER_ELEMENT);
void* new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT);

#endif