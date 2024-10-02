
#![allow(incomplete_features,unused_attributes)]
#![feature(specialization)]
use crate::*;
use std::ffi::c_void;


#[link(name="cvec",kind="static")]
#[allow(dead_code)]
extern "C" {
  pub(crate) fn vec_new(bytes_per_element: usize,vtable: VecVTable)-> RawCVec;
  pub(crate) fn vec_with_capacity(capacity: usize,bytes_per_element: usize,vtable: VecVTable)-> RawCVec;
  pub(crate) fn drop_vec(this: &mut RawCVec);
  pub(crate) fn vec_push(this: &mut RawCVec,element: *mut c_void);
  pub(crate) fn vec_pop(this: &mut RawCVec);
  pub(crate) fn vec_reserve(this: &mut RawCVec,additional: usize);
  pub(crate) fn vec_reserve_exact(this: &mut RawCVec,additional: usize);
  pub(crate) fn vec_append(this: &mut RawCVec,other: *mut c_void);
  pub(crate) fn vec_extend(this: &mut RawCVec,data: *mut c_void,len: usize);
  pub(crate) fn vec_clear(this: &mut RawCVec);
  pub(crate) fn vec_insert(this: &mut RawCVec,index: usize,element: *mut c_void);
  pub(crate) fn vec_remove(this: &mut RawCVec,index: usize)-> *mut c_void;
  pub(crate) fn vec_truncate(this: &mut RawCVec,len: usize);
  pub(crate) fn vec_resize(this: &mut RawCVec,new_len: usize,value: *mut c_void);
  pub(crate) fn vec_resize_with(this: &mut RawCVec,new_len: usize,f: extern "C" fn(*mut c_void)-> *mut c_void);
  pub(crate) fn vec_retain(this: &mut RawCVec,f: extern "C" fn(*mut c_void)-> bool);
}


pub(crate) extern "C" fn destructor<T>(this: *mut c_void) {
  unsafe {
    std::ptr::drop_in_place(this as *mut T)
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

pub(crate) trait Clonable<T> {
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
      std::mem::transmute(0usize)
    }
  }
}

