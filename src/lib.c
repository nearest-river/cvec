#include "prelude.h"
#include "alloc.h"
#include <string.h>


Vec new_vec(usize BYTES_PER_ELEMENT,Destructor destructor) {
  return new_vec_with_capacity(DEFAULT_CAPACITY,BYTES_PER_ELEMENT,destructor);
}

Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT,Destructor destructor) {
  Vec self;

  self.BYTES_PER_ELEMENT=BYTES_PER_ELEMENT;
  self.capacity=capacity;
  self.len=0;
  self.ptr=alloc(capacity*BYTES_PER_ELEMENT);
  self.destructor=destructor;

  return self;
}

void drop_vec(Self self) {
  Vec this=*self;

  if(this.destructor) {
    _drop_in_place(this.ptr,this.len,this.BYTES_PER_ELEMENT,this.destructor);
  }
  free(self->ptr);
}


void vec_push(Self self,void* element) {
  not_null2(self,element);
  Vec this=*self;

  if(this.capacity==this.len) {
    vec_reserve(self,this.capacity);
  }

  memmove(self->ptr+(this.BYTES_PER_ELEMENT*self->len++),element,this.BYTES_PER_ELEMENT);
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

  memmove(element,this.ptr+(--self->len*this.BYTES_PER_ELEMENT),this.BYTES_PER_ELEMENT);
  return element;
}

void vec_append(Self self,Vec* other) {
  not_null2(self,other);
  Vec this=*self;
  Vec other_vec=*other;

  assert(this.BYTES_PER_ELEMENT==other_vec.BYTES_PER_ELEMENT);

  vec_reserve(self,other_vec.len);
  memmove(self->ptr+(this.BYTES_PER_ELEMENT*this.len),other_vec.ptr,this.BYTES_PER_ELEMENT*other_vec.len);

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
  if(this.destructor) {
    _drop_in_place(this.ptr,this.len,this.BYTES_PER_ELEMENT,this.destructor);
  }

  self->len=0;
}

void vec_insert(Self self,usize index,void* element) {
  usize len=self->len++;
  Vec this=*self;

  if(len==this.capacity) vec_reserve(self,1);
  void* dest=self->ptr+(index*this.BYTES_PER_ELEMENT);

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
  void* ptr=this.ptr+(index*this.BYTES_PER_ELEMENT);
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

  if(!this.destructor) return;
  usize count=this.len-len;// old_len-new_len
  _drop_in_place(this.ptr,count,this.BYTES_PER_ELEMENT,this.destructor);
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

  void* ptr=self->ptr+(this.len*this.BYTES_PER_ELEMENT);
  for(usize i=0;i<required_len;i++) {
    memmove(ptr,value,this.BYTES_PER_ELEMENT);
    ptr+=this.BYTES_PER_ELEMENT;
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

  void* ptr=self->ptr+(this.len*this.BYTES_PER_ELEMENT);
  for(usize i=0;i<required_len;i++) {
    void* value=f();

    memmove(ptr,value,this.BYTES_PER_ELEMENT);
    free(value);
    ptr+=this.BYTES_PER_ELEMENT;
  }
}


