
use crate::bindings::*;
use std::{
  mem,
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
pub(crate) struct RawCVec {
  bytes_per_element: usize,
  len: usize,
  capacity: usize,
  vtable: VecVTable,
  ptr: *mut c_void,
}

/**
 * A function that frees the resources held by `self`.
 */
pub(crate) type Destructor=extern "C" fn(*mut c_void);

/**
 * A function that clones `self` to `dest` without forgetting about the resources held by `self`.
 * 
 * * Params: `(void* self,void* dest)`.
 */
pub(crate) type Cloner=extern "C" fn(*mut c_void,*mut c_void);

/**
 * This virtual table keeping track of the resources held by the `CVec`.
 */
#[repr(C)]
pub(crate) struct VecVTable {
  destructor: Destructor,
  cloner: Cloner
}


/**
 * The `Slice` type.
 * It holds a reference to some contigious data somewhere in the memory.
 */
#[allow(dead_code)]
#[repr(C)]
pub(crate) struct Slice {
  bytes_per_element: usize,
  len: usize,
  data: *mut c_void,
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