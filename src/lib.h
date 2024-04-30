#ifndef VEC_LIB_H
#define VEC_LIB_H



typedef __SIZE_TYPE__ usize;

/**
 * `Vec` shands for `vector`
 */
typedef struct Vec {
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
  void* ptr;
} Vec;


Vec new_vec(usize BYTES_PER_ELEMENT);
Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT);
void vec_reserve(Self self,usize additional);

#endif