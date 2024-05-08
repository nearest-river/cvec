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

void _vec_grow_amortized(Vec* self,usize capacity,usize len,usize additional) {
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  const usize MIN_NON_ZERO_CAPACITY=MIN_NON_ZERO_CAP(BYTES_PER_ELEMENT);

  usize required_cap=len+additional;
  usize cap0=max(capacity*2,required_cap);
  usize cap=max(cap0,MIN_NON_ZERO_CAPACITY);

  void* prev_ptr=self->ptr;
  void* new_ptr=realloc(prev_ptr,cap*BYTES_PER_ELEMENT);

  not_null(new_ptr);
  self->capacity=cap;

  if(new_ptr==prev_ptr) return;

  self->ptr=new_ptr;
  free(prev_ptr);
}



void vec_reserve_exact(Self self,usize additional) {
  not_null(self);
  if(self->capacity-self->len >= additional) return;

  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  




}
