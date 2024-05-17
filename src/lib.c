#include "prelude.h"
#include "alloc.h"
#include <string.h>

typedef Vec* Self;

Vec new_vec(usize BYTES_PER_ELEMENT,VecVTable vtable) {
  return new_vec_with_capacity(DEFAULT_CAPACITY,BYTES_PER_ELEMENT,vtable);
}

Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT,VecVTable vtable) {
  Vec self={
    .BYTES_PER_ELEMENT=BYTES_PER_ELEMENT,
    .len=0,
    .capacity=capacity,
    .vtable=vtable,
    .ptr=alloc(capacity*BYTES_PER_ELEMENT)
  };

  return self;
}

void drop_vec(Self self) {
  Vec this=*self;


  _drop_in_place(this.ptr,this.len,this.BYTES_PER_ELEMENT,this.vtable.destructor);
  free(self->ptr);
}


void vec_push(Self self,void* element) {
  not_null2(self,element);
  Vec this=*self;

  if(this.capacity==this.len) {
    vec_reserve(self,this.capacity);
  }

  memmove(ptr__index(self->ptr,this.BYTES_PER_ELEMENT,self->len++),element,this.BYTES_PER_ELEMENT);
}

void vec_reserve(Self self,usize additional) {
  not_null(self);
  Vec this=*self;

  if(this.capacity-this.len < additional) {
    _vec_grow_amortized(self,this.capacity,this.len,additional);
  }
}

void vec_reserve_exact(Self self,usize additional) {
  not_null(self);
  Vec this=*self;
  if(this.capacity-this.len >= additional) return;

  void* new_ptr=realloc(this.ptr,(additional+this.capacity)*this.BYTES_PER_ELEMENT);
  if(new_ptr==this.ptr) return;

  self->ptr=new_ptr;
}

void* vec_pop(Self self) {
  not_null(self);
  Vec this=*self;
  void* element=alloc(this.BYTES_PER_ELEMENT);

  memmove(element,vec__index(this,--self->len),this.BYTES_PER_ELEMENT);
  return element;
}

void vec_append(Self self,Vec* other) {
  not_null2(self,other);
  Vec this=*self;
  Vec other_vec=*other;

  assert(this.BYTES_PER_ELEMENT==other_vec.BYTES_PER_ELEMENT);

  vec_reserve(self,other_vec.len);
  memmove(ptr__index(self->ptr,this.BYTES_PER_ELEMENT,this.len),other_vec.ptr,this.BYTES_PER_ELEMENT*other_vec.len);

  self->len+=other_vec.len;
  other->len=0;
}

void vec_extend(Self self,void* data,usize len) {
  not_null2(self,data);
  Vec this=*self;

  vec_reserve(self,len);
  memcpy(self->ptr+(this.BYTES_PER_ELEMENT*this.len),data,this.BYTES_PER_ELEMENT*len);

  self->len+=len;
}

void vec_clear(Self self) {
  not_null(self);
  Vec this=*self;
  _drop_in_place(this.ptr,this.len,this.BYTES_PER_ELEMENT,this.vtable.destructor);


  self->len=0;
}

void vec_insert(Self self,usize index,void* element) {
  usize len=self->len++;
  Vec this=*self;

  if(len==this.capacity) vec_reserve(self,1);
  void* dest=ptr__index(self->ptr,index,this.BYTES_PER_ELEMENT);

  if(index<len) {
    // Shift everything over to make space.
    memmove(dest+this.BYTES_PER_ELEMENT,dest,this.BYTES_PER_ELEMENT*(len-index));
  } else if(index>len) {
    panic("insertion index (is %ld) should be <= len (is %ld)",index,len);
  }
  // index==len needs no elements to be shifted.

  memmove(dest,element,this.BYTES_PER_ELEMENT);
}

void* vec_remove(Self self,usize index) {
  not_null(self);
  Vec this=*self;

  if(index>=this.len) panic("removal index (is %ld) should be < len (is %ld)",index,this.len);
  void* ptr=vec__index(this,index);
  void* ret=malloc(this.BYTES_PER_ELEMENT);

  memmove(ret,ptr,this.BYTES_PER_ELEMENT);
  memmove(ptr,ptr+this.BYTES_PER_ELEMENT,this.BYTES_PER_ELEMENT*(this.len-index-1));// Shifting
  self->len--;

  return ret;
}

void vec_truncate(Self self,usize len) {
  not_null(self);
  Vec this=*self;

  if(len>this.len) return;
  self->len=len;
  _drop_in_place(this.ptr,this.len-len,this.BYTES_PER_ELEMENT,this.vtable.destructor);
}

void vec_resize(Self self,usize new_len,void* value) {
  not_null(self);
  Vec this=*self;

  if(new_len<=this.len) {
    vec_truncate(self,new_len);
    return;
  }

  usize required_len=new_len-this.len;
  vec_reserve(self,required_len);

  void* ptr=ptr__index(self->ptr,this.len,this.BYTES_PER_ELEMENT);

  if(this.vtable.cloner) {
    for(usize i=0;i<required_len;i++) {
      this.vtable.cloner(ptr,value);
      ptr+=this.BYTES_PER_ELEMENT;
    }
  } else {
    for(usize i=0;i<required_len;i++) {
      memmove(ptr,value,this.BYTES_PER_ELEMENT);
      ptr+=this.BYTES_PER_ELEMENT;
    }
  }

  self->len=new_len;
}

void vec_resize_with(Self self,usize new_len,void* (*f)(void)) {
  not_null2(self,f);
  Vec this=*self;

  if(new_len<=this.len) {
    vec_truncate(self,new_len);
    return;
  }

  usize required_len=new_len-this.len;
  vec_reserve(self,required_len);

  void* ptr=ptr__index(self->ptr,this.len,this.BYTES_PER_ELEMENT);
  for(usize i=0;i<required_len;i++) {
    void* value=f();

    memmove(ptr,value,this.BYTES_PER_ELEMENT);
    free(value);
    ptr+=this.BYTES_PER_ELEMENT;
  }
}

void vec_retain(Self self,bool (*f)(void*)) {
  Vec this=*self;
  usize len=0;

  void* ptr=this.ptr;
  for(usize i=0;i<this.len;i++,ptr+=this.BYTES_PER_ELEMENT) {
    if(!f(ptr)) {
      if(this.vtable.destructor) this.vtable.destructor(ptr);
      continue;
    }

    memmove(vec__index(this,len++),ptr,this.BYTES_PER_ELEMENT);
  }

  self->len=len;
}

void vec_shrink_to(Self self,usize min_capacity) {
  Vec this=*self;
  if(this.capacity<=min_capacity) return;
  _drop_in_place(vec__index(this,min_capacity),this.capacity-min_capacity,this.BYTES_PER_ELEMENT,this.vtable.destructor);

  self->ptr=realloc(this.ptr,min_capacity*this.BYTES_PER_ELEMENT);
}


void vec_shrink_to_fit(Self self) {
  vec_shrink_to(self,self->len);
}

Slice vec_spare_capacity(Self self) {
  Vec this=*self;
  Slice slice={
    .BYTES_PER_ELEMENT=this.BYTES_PER_ELEMENT,
    .data=vec__index(this,this.len),
    .len=this.capacity-this.capacity
  };

  return slice;
}

void* vec_swap_remove(Self self,usize index) {
  Vec this=*self;
  assert(this.len>index);

  void* ret=alloc(this.BYTES_PER_ELEMENT);
  void* src=vec__index(this,index);

  if(this.vtable.cloner) {
    this.vtable.cloner(ret,src);
    this.vtable.cloner(src,vec__index(this,this.len));
  } else {
    memmove(ret,src,this.BYTES_PER_ELEMENT);
    memmove(src,vec__index(this,this.len),this.BYTES_PER_ELEMENT);
  }

  return ret;
}

