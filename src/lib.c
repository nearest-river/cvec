#include "prelude.h"
#include "alloc.h"
#include <string.h>
#include <assert.h>

#define DEFAULT_SIZE 16
#define as_vec(ptr) ((Vec*)ptr)-1
#define non_null2(ptr1,ptr2) assert(ptr1!=NULL);assert(ptr2!=NULL)

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
