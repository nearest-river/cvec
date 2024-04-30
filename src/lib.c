#include "prelude.h"
#include "alloc.h"
#include <string.h>

#define needs_to_grow(ptr,additional) self->capacity-self->len < additional

typedef Vec* Self;


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
void vec_push(void* self,void* element) {
  not_null2(self,element);

}

void vec_reserve(Self self,usize additional) {
  not_null(self);
  if(needs_to_grow(self,additional)) {
    _vec_grow_amortized(self,additional);
  }
}




