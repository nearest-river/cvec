#include "prelude.h"
#include "alloc.h"



inline void* __alloc(usize size) {
  void* ptr=malloc(size);
  if(!ptr) panic("Could'nt allocate memory.\nmalloc returned `NULL` pointer\n");

  return ptr;
}

bool _vec_grow_amortized(Vec* self,usize additional) {
  usize MIN_NON_ZERO_CAPACITY=MIN_NON_ZERO_CAP(self->BYTES_PER_ELEMENT);

  usize required_cap=self->len+additional;
  usize cap0=max(self->capacity*2,required_cap);
  usize cap=max(cap0,MIN_NON_ZERO_CAPACITY);

  void* new_ptr=realloc(self->ptr,cap*self->BYTES_PER_ELEMENT);
  if(!new_ptr) return false;

  self->capacity=cap;
  self->ptr=new_ptr;
  return true;
}

void _drop_in_place(void* ptr,usize count,usize BYTES_PER_ELEMENT,Destructor destructor) {
  if(!destructor) return;

  for(usize i=0;i<count;i++) {
    destructor(ptr);
    ptr+=BYTES_PER_ELEMENT;
  }
}

