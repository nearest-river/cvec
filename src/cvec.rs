
use std::{
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

