#include "prelude.h"
#include <string.h>



void* alloc(usize size) {
  void* ptr=malloc(size);
  if(!ptr) panic("Could'nt allocate memory.\nmalloc returned `NULL` pointer");

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
}

void _drop_in_place(void* ptr,usize count,usize BYTES_PER_ELEMENT,void (*destructor)(void*)) {
  for(usize i=0;i<count;i++) {
    destructor(ptr);
    ptr+=BYTES_PER_ELEMENT;
  }
}

