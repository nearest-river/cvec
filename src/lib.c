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

void vec_reserve_exact(Self self,usize additional) {
  not_null(self);
  usize capacity=self->capacity;
  if(capacity-self->len >= additional) return;

  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;

  void* prev_ptr=self->ptr;
  void* new_ptr=realloc(prev_ptr,(additional+capacity)*BYTES_PER_ELEMENT);

  if(new_ptr==prev_ptr) return;

  free(self->ptr);
  self->ptr=new_ptr;
}

void* vec_pop(Self self) {
  not_null(self);
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  void* element=alloc(BYTES_PER_ELEMENT);

  memmove(element,self->ptr+(--self->len*BYTES_PER_ELEMENT),BYTES_PER_ELEMENT);
  return element;
}

void vec_append(Self self,Vec* other) {
  not_null2(self,other);

  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;
  assert(BYTES_PER_ELEMENT==other->BYTES_PER_ELEMENT);

  vec_reserve(self,other->len);
  memmove(self->ptr+(BYTES_PER_ELEMENT*self->len),other->ptr,BYTES_PER_ELEMENT*other->len);

  self->len+=other->len;
  other->len=0;
}

void vec_extend(Self self,void* data,usize len) {
  not_null2(self,data);
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;

  vec_reserve(self,len);
  memcpy(self->ptr+(BYTES_PER_ELEMENT*self->len),data,len*BYTES_PER_ELEMENT);

  self->len+=len;
}

void vec_clear(Self self,void (*destructor)(void*)) {
  not_null(self);
  Vec this=*self;
  if(destructor!=NULL) {
    _drop_in_place(this.ptr,this.len,this.BYTES_PER_ELEMENT,destructor);
  }

  self->len=0;
}

void vec_insert(Self self,usize index,void* element) {
  usize len=self->len++;
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;

  if(len==self->capacity) vec_reserve(self,1);
  void* dest=self->ptr+(index*BYTES_PER_ELEMENT);

  if(index<len) {
    // Shift everything over to make space.
    memmove(dest+BYTES_PER_ELEMENT,dest,BYTES_PER_ELEMENT*(len-index));
  } else if(index>len) {
    panic("insertion index (is %ld) should be <= len (is %ld)",index,len);
  }
  // index==len needs no elements to be shifted.

  memmove(dest,element,BYTES_PER_ELEMENT);
}

void* vec_remove(Self self,usize index) {
  not_null(self);
  usize len=self->len--;
  const usize BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT;

  if(index>=len) panic("removal index (is %ld) should be < len (is %ld)",index,len);
  void* ptr=self->ptr+(index*BYTES_PER_ELEMENT);
  void* ret=malloc(BYTES_PER_ELEMENT);

  memmove(ret,ptr,BYTES_PER_ELEMENT);
  memmove(ptr,ptr+BYTES_PER_ELEMENT,BYTES_PER_ELEMENT*(len-index-1));// Shifting

  return ret;
}

void vec_truncate(Self self,usize len,void (*destructor)(void*)) {
  not_null(self);
  Vec this=*self;

  if(len>this.len) return;
  self->len=len;

  if(!destructor) return;
  usize remaining_len=this.len-len;
  _drop_in_place(this.ptr,remaining_len,this.BYTES_PER_ELEMENT,destructor);
}

void vec_resize(Self self,usize new_len,void* value,void (*destructor)(void*)) {
  not_null(self);
  Vec this=*self;

  if(new_len<=this.len) {
    vec_truncate(self,new_len,destructor);
    return;
  }

  usize required_len=new_len-this.len;
  vec_reserve(self,required_len);

  void* ptr=this.ptr+(this.len*this.BYTES_PER_ELEMENT);
  for(usize i=0;i<required_len;i++) {
    memmove(value,ptr,this.BYTES_PER_ELEMENT);
    ptr+=this.BYTES_PER_ELEMENT;
  }

  self->len=new_len;
}




