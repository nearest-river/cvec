#include "prelude.h"

typedef __SIZE_TYPE__ usize;


typedef struct Vec {
  void* ptr;
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
} Vec;


Vec new_vec(usize BYTES_PER_ELEMENT);
Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT);



