#include "prelude.h"
#include "alloc.h"

#define DEFAULT_SIZE 16

void* new_vec(usize BYTES_PER_ELEMENT) {
  return new_vec_with_capacity(DEFAULT_SIZE,BYTES_PER_ELEMENT);
}

void* new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT) {
  Vec* self=alloc(sizeof(Vec)+(capacity*BYTES_PER_ELEMENT));

  self->BYTES_PER_ELEMENT=BYTES_PER_ELEMENT;
  self->capacity=capacity;
  self->len=0;

  return self+1;
}










