#include "prelude.h"
#include <string.h>



void* alloc(usize size) {
  void* ptr=malloc(size);

  if(!ptr) {
    printf("Could'nt allocate memory.\nmalloc returned `NULL` pointer");
    exit(1);
  }

  return ptr;
}

void _vec_grow_amortized(Vec* self,usize additional) {
  usize required_cap=self->len+additional;
  usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;

  usize cap=max(self->capacity*2,required_cap);
  usize cap=max(cap,MIN_NON_ZERO_CAP(BYTES_PER_ELEMENT));
  usize layout_size=cap*BYTES_PER_ELEMENT;

  void* prev_ptr=self->ptr;
  void* new_ptr=realloc(prev_ptr,layout_size);

  not_null(new_ptr);
  self->capacity=cap;
  if(new_ptr==prev_ptr) return;

  memmove(new_ptr,prev_ptr,layout_size);
}



