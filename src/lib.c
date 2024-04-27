#include "prelude.h"
#include "alloc.h"

#define DEFAULT_SIZE 16


Vec new_vec(usize BYTES_PER_ELEMENT) {
  return new_vec_with_capacity(DEFAULT_SIZE,BYTES_PER_ELEMENT);
}

Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT) {
  Vec self;

  self.ptr=alloc(capacity*BYTES_PER_ELEMENT);
  self.BYTES_PER_ELEMENT=BYTES_PER_ELEMENT;
  self.capacity=16;
  self.len=0;

  return self;
}










