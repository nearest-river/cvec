#ifndef VEC_LIB_H
#define VEC_LIB_H

#ifdef _cplusplus
extern "C" {
#endif

typedef __SIZE_TYPE__ usize;

/**
 * `Vec` shands for `vector`
 */
typedef struct Vec {
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
  void* ptr;
} Vec;
typedef Vec* Self;


/**
 * Constructs a new empty `Vec` with a capacity.
 */
Vec new_vec(usize BYTES_PER_ELEMENT);
/**
 * Construcs a new `Vec` with the specefied capacity.
 */
Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT);

/**
 * Pushes an element to the back of the `Vec`.
 * 
 * * `element` is moved afterwords.
 * * It's the user's responsiblity to free `element`.
 */
void vec_push(Self self,void* element);

/**
 * Pops the last element of the `Vec`
 * 
 * * The element is moved afterwords.
 * * It's the user's responsiblity to free `element`.
 */
void* vec_pop(Self self);

/**
 * Reserves capacity for at least `additional` more elements to be inserted in the given `Vec`.
 * The collection may reserve more space to speculatively avoid frequent reallocations.
 * After calling reserve, capacity will be greater than or equal to `len` + `additional`.
 * Does nothing if capacity is already sufficient.
 * 
 * ## Panics
 * Panics if the new capacity exceeds 0x7fffffff bytes.
 */
void vec_reserve(Self self,usize additional);

/**
 * Reserves the minimum capacity for at least additional more elements to be inserted in the given `Vec`.
 * Unlike reserve, this will not deliberately over-allocate to speculatively avoid frequent allocations.
 * After calling `vec_reserve_exact`, capacity will be greater than or equal to self.len() + additional.
 * Does nothing if the capacity is already sufficient.
 * 
 * * Note that the allocator may give the collection more space than it requests.
 * Therefore, capacity can not be relied upon to be precisely minimal.
 * Prefer `vec_reserve` if future insertions are expected.
 * 
 * ## Panics
 * Panics if the new capacity exceeds 0x7fffffff.
 */
void vec_reserve_exact(Self self,usize additional);

#ifdef _cplusplus
}
#endif
#endif