
#![allow(incomplete_features,unused_attributes)]
#![feature(specialization)]
use std::{
  mem,
  ptr,
  ffi::c_void,
  marker::PhantomData
};


/**
 * `CVec` stands for `vector`.
 */
pub struct CVec<T> {
  inner: RawCVec,
  _marker: PhantomData<T>
}


#[repr(C)]
struct RawCVec {
  bytes_per_element: usize,
  len: usize,
  capacity: usize,
  vtable: VecVTable,
  ptr: *mut c_void,
}

/**
 * A function that frees the resources held by `self`.
 */
pub type Destructor=extern "C" fn(*mut c_void);

/**
 * A function that clones `self` to `dest` without forgetting about the resources held by `self`.
 * 
 * * Params: `(void* self,void* dest)`.
 */
pub type Cloner=extern "C" fn(*mut c_void,*mut c_void);

/**
 * This virtual table keeping track of the resources held by the `CVec`.
 */
#[repr(C)]
pub struct VecVTable {
  destructor: Destructor,
  cloner: Cloner
}


/**
 * The `Slice` type.
 * It holds a reference to some contigious data somewhere in the memory.
 */
#[allow(dead_code)]
#[repr(C)]
pub struct Slice {
  bytes_per_element: usize,
  len: usize,
  data: *mut c_void,
}


#[link(name="cvec",kind="static")]
#[allow(dead_code)]
extern "C" {
  fn new_vec(bytes_per_element: usize,vtable: VecVTable)-> RawCVec;
  fn new_vec_with_capacity(capacity: usize,bytes_per_element: usize,vtable: VecVTable)-> RawCVec;
  fn drop_vec(this: &mut RawCVec);
  fn vec_push(this: &mut RawCVec,element: *mut c_void);
  fn vec_pop(this: &mut RawCVec);
  fn vec_reserve(this: &mut RawCVec,additional: usize);
  fn vec_reserve_exact(this: &mut RawCVec,additional: usize);
  fn vec_append(this: &mut RawCVec,other: *mut c_void);
  fn vec_extend(this: &mut RawCVec,data: *mut c_void,len: usize);
  fn vec_clear(this: &mut RawCVec);
  fn vec_insert(this: &mut RawCVec,index: usize,element: *mut c_void);
  fn vec_remove(this: &mut RawCVec,index: usize)-> *mut c_void;
  fn vec_truncate(this: &mut RawCVec,len: usize);
  fn vec_resize(this: &mut RawCVec,new_len: usize,value: *mut c_void);
  fn vec_resize_with(this: &mut RawCVec,new_len: usize,f: extern "C" fn(*mut c_void)-> *mut c_void);
  fn vec_retain(this: &mut RawCVec,f: extern "C" fn(*mut c_void)-> bool);
}

extern "C" fn destructor<T>(this: *mut c_void) {
  unsafe {
    ptr::drop_in_place(this as *mut T)
  };
}

extern "C" fn _cloner<T: Clone>(dest: *mut c_void,src: *mut c_void) {
  let (dest,src)=unsafe {(
    (dest as *mut T).as_mut(),
    (src as *mut T).as_ref())
  };

  match (dest,src) {
    (Some(dest),Some(src))=> Clone::clone_from(dest,src as _),
    _=> return
  }
}

trait Clonable<T> {
  fn cloner()-> Cloner;
}

impl<T: Clone> Clonable<T> for T {
  fn cloner()-> Cloner {
    _cloner::<T>
  }
}

impl<T> Clonable<T> for T {
  #[allow(invalid_value)]
  default fn cloner()-> Cloner {
    // SAFETY: inner implementation always checks for null value.
    unsafe {
      mem::transmute(0usize)
    }
  }
}


impl<T> CVec<T> {
  pub const DEFAULT_CAPACITY: usize=0;
  pub fn new()-> CVec<T> {
    CVec {
      inner: unsafe {
        new_vec(
          mem::size_of::<T>(),
          VecVTable {
            destructor: destructor::<T>,
            cloner: T::cloner()
          }
        )
      },
      _marker: PhantomData
    }
  }

  pub fn with_capacity(capacity: usize)-> CVec<T> {
    CVec {
      inner: unsafe {
        new_vec_with_capacity(
          capacity,
          mem::size_of::<T>(),
          VecVTable {
            cloner: T::cloner(),
            destructor: destructor::<T>
          }
        )
      },
      _marker: PhantomData
    }
  }

  pub fn len(&self)-> usize {
    self.inner.len
  }

  pub fn push(&mut self,element: T) {
    // SAFETY: This c-function is safe.
    unsafe {
      vec_push(&mut self.inner,&element as *const _ as *mut _);
    }
  }

  pub fn retain(&mut self,f: extern "C" fn(&T)-> bool) {
    // SAFETY: This c-function is safe.
    unsafe {
      vec_retain(&mut self.inner,mem::transmute(f))
    }
  }
}


impl<T> Drop for CVec<T> {
  fn drop(&mut self) {
    // SAFETY: This c-function is safe.
    unsafe {
      drop_vec(&mut self.inner)
    }
  }
}

impl<T> AsRef<[T]> for CVec<T> {
  fn as_ref(&self)-> &[T] {
    // SAFETY: `self.ptr` is always a valid pointer.
    unsafe {
      std::slice::from_raw_parts(self.inner.ptr as _,self.inner.len)
    }
  }
}

impl<T> AsMut<[T]> for CVec<T> {
  fn as_mut(&mut self)-> &mut [T] {
    // SAFETY: `self.ptr` is always a valid pointer.
    unsafe {
      std::slice::from_raw_parts_mut(self.inner.ptr as _,self.inner.len)
    }
  }
}
