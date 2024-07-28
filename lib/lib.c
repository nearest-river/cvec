#include "prelude.h"
#include "alloc.h"
#include <string.h>

typedef Vec* Self;

inline Vec new_vec(usize BYTES_PER_ELEMENT,VecVTable vtable) {
  return new_vec_with_capacity(DEFAULT_CAPACITY,BYTES_PER_ELEMENT,vtable);
}

inline Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT,VecVTable vtable) {
  Vec self={
    .BYTES_PER_ELEMENT=BYTES_PER_ELEMENT,
    .len=0,
    .capacity=capacity,
    .vtable=vtable,
    .ptr=__alloc(capacity*BYTES_PER_ELEMENT)
  };

  return self;
}

void drop_vec(Self self) {
  _drop_in_place(self->ptr,self->len,self->BYTES_PER_ELEMENT,self->vtable.destructor);
  free(self->ptr);
}


void vec_push(Self self,void* element) {
  not_null2(self,element);

  if(self->capacity==self->len) {
    vec_reserve(self,self->capacity);
  }

  memmove(vec__index__ptr(self,self->len++),element,self->BYTES_PER_ELEMENT);
}

void vec_reserve(Self self,usize additional) {
  not_null(self);

  if(self->capacity-self->len < additional) {
    _vec_grow_amortized(self,additional);
  }
}

void vec_reserve_exact(Self self,usize additional) {
  not_null(self);
  if(self->capacity-self->len >= additional) return;
  void* new_ptr=realloc(self->ptr,(additional+self->capacity)*self->BYTES_PER_ELEMENT);

  if(new_ptr==self->ptr) return;
  self->ptr=new_ptr;
}

void* vec_pop(Self self) {
  not_null(self);
  void* element=__alloc(self->BYTES_PER_ELEMENT);

  memmove(element,vec__index__ptr(self,--self->len),self->BYTES_PER_ELEMENT);
  return element;
}

void vec_append(Self self,Vec* other) {
  not_null2(self,other);
  assert(self->BYTES_PER_ELEMENT==other->BYTES_PER_ELEMENT);

  vec_reserve(self,other->len);
  memmove(vec__index__ptr(self,self->len),other->ptr,self->BYTES_PER_ELEMENT*other->len);

  self->len+=other->len;
  other->len=0;
}

void vec_extend(Self self,void* data,usize len) {
  not_null2(self,data);

  vec_reserve(self,len);
  memmove(self->ptr+(self->BYTES_PER_ELEMENT*self->len),data,self->BYTES_PER_ELEMENT*len);

  self->len+=len;
}

void vec_clear(Self self) {
  not_null(self);

  _drop_in_place(self->ptr,self->len,self->BYTES_PER_ELEMENT,self->vtable.destructor);
  self->len=0;
}

void vec_insert(Self self,usize index,void* element) {
  usize len=self->len++;

  if(len==self->capacity) vec_reserve(self,1);
  void* dest=vec__index__ptr(self,index);

  if(index<len) {
    // Shift everything over to make space.
    memmove(dest+self->BYTES_PER_ELEMENT,dest,self->BYTES_PER_ELEMENT*(len-index));
  } else if(index>len) {
    panic("insertion index (is %ld) should be <= len (is %ld)",index,len);
  }
  // index==len needs no elements to be shifted.

  memmove(dest,element,self->BYTES_PER_ELEMENT);
}

void* vec_remove(Self self,usize index) {
  not_null(self);

  if(index>=self->len) panic("removal index (is %ld) should be < len (is %ld)",index,self->len);
  void* ptr=vec__index__ptr(self,index);
  void* ret=__alloc(self->BYTES_PER_ELEMENT);

  memmove(ret,ptr,self->BYTES_PER_ELEMENT);
  memmove(ptr,ptr+self->BYTES_PER_ELEMENT,self->BYTES_PER_ELEMENT*(self->len-index-1));// Shifting
  self->len--;

  return ret;
}

void vec_truncate(Self self,usize len) {
  not_null(self);
  if(len>self->len) return;

  self->len=len;
  _drop_in_place(self->ptr,self->len-len,self->BYTES_PER_ELEMENT,self->vtable.destructor);
}

void vec_resize(Self self,usize new_len,void* value) {
  not_null(self);
  if(new_len<=self->len) {
    vec_truncate(self,new_len);
    return;
  }

  usize required_len=new_len-self->len;
  vec_reserve(self,required_len);

  void* ptr=vec__index__ptr(self,self->len);

  if(self->vtable.cloner) {
    for(usize i=0;i<required_len;i++) {
      self->vtable.cloner(ptr,value);
      ptr+=self->BYTES_PER_ELEMENT;
    }
  } else {
    for(usize i=0;i<required_len;i++) {
      memmove(ptr,value,self->BYTES_PER_ELEMENT);
      ptr+=self->BYTES_PER_ELEMENT;
    }
  }

  self->len=new_len;
}

void vec_resize_with(Self self,usize new_len,void* (*f)(void)) {
  not_null2(self,f);
  if(new_len<=self->len) {
    vec_truncate(self,new_len);
    return;
  }

  usize required_len=new_len-self->len;
  vec_reserve(self,required_len);

  void* ptr=vec__index__ptr(self,self->len);
  for(usize i=0;i<required_len;i++) {
    void* value=f();

    memmove(ptr,value,self->BYTES_PER_ELEMENT);
    free(value);
    ptr+=self->BYTES_PER_ELEMENT;
  }
}

void vec_retain(Self self,bool (*f)(void*)) {
  usize len=0;

  void* ptr=self->ptr;
  for(usize i=0;i<self->len;i++,ptr+=self->BYTES_PER_ELEMENT) {
    if(!f(ptr)) {
      if(self->vtable.destructor) self->vtable.destructor(ptr);
      continue;
    }

    memmove(vec__index__ptr(self,len++),ptr,self->BYTES_PER_ELEMENT);
  }

  self->len=len;
}

void vec_shrink_to(Self self,usize min_capacity) {
  if(self->capacity<=min_capacity) return;

  _drop_in_place(vec__index__ptr(self,min_capacity),self->capacity-min_capacity,self->BYTES_PER_ELEMENT,self->vtable.destructor);
  self->ptr=realloc(self->ptr,min_capacity*self->BYTES_PER_ELEMENT);
}


void vec_shrink_to_fit(Self self) {
  vec_shrink_to(self,self->len);
}

inline Slice vec_spare_capacity(Self self) {
  Slice slice={
    .BYTES_PER_ELEMENT=self->BYTES_PER_ELEMENT,
    .data=vec__index__ptr(self,self->len),
    .len=self->capacity-self->capacity
  };

  return slice;
}

void* vec_swap_remove(Self self,usize index) {
  assert(self->len>index);
  void* ret=__alloc(self->BYTES_PER_ELEMENT);
  void* src=vec__index__ptr(self,index);

  if(self->vtable.cloner) {
    self->vtable.cloner(ret,src);
    self->vtable.cloner(src,vec__index__ptr(self,self->len));
  } else {
    memmove(ret,src,self->BYTES_PER_ELEMENT);
    memmove(src,vec__index__ptr(self,self->len),self->BYTES_PER_ELEMENT);
  }

  return ret;
}

