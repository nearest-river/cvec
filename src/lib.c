#include "prelude.h"
#include "alloc.h"
#include <string.h>


Vec new_vec(usize BYTES_PER_ELEMENT) {
  return new_vec_with_capacity(DEFAULT_SIZE,BYTES_PER_ELEMENT);
}

Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT) {
  Vec self;

  self.BYTES_PER_ELEMENT=BYTES_PER_ELEMENT;
  self.capacity=capacity;
  self.len=0;
  self.ptr=alloc(capacity*BYTES_PER_ELEMENT);

  return self;
}

/**
 * Pushes an element to the back of the `Vec`.
 * 
 * * `element` is moved afterwords.
 * * It's the user's responsiblity to free `element`.
 */
void vec_push(Self self,void* element) {
  not_null2(self,element);
  usize capacity=self->capacity;

  if(capacity==self->len) {
    vec_reserve(self,capacity);
  }

  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  memmove(self->ptr+(BYTES_PER_ELEMENT*self->len++),element,BYTES_PER_ELEMENT);
}

void vec_reserve(Self self,usize additional) {
  not_null(self);
  usize capacity=self->capacity;
  usize len=self->len;
  if(capacity-len < additional) {
    _vec_grow_amortized(self,capacity,len,additional);
  }
}

void* vec_pop(Self self) {
  not_null(self);
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  void* element=alloc(BYTES_PER_ELEMENT);

  memmove(element,self->ptr+(--self->len*BYTES_PER_ELEMENT),BYTES_PER_ELEMENT);
  return element;
}





