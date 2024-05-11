#ifndef VEC_LIB_H
#define VEC_LIB_H

#ifdef _cplusplus
extern "C" {
#endif

#define MAX_CAPACITY 0x7fffffffUL

typedef __SIZE_TYPE__ usize;

/**
 * `Vec` shands for `vector`
 */
typedef struct Vec {
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
  void* ptr;
  void (*destructor)(void*);
} Vec;
typedef Vec* Self;


/**
 * Constructs a new empty `Vec` with a capacity.
 */
Vec new_vec(usize BYTES_PER_ELEMENT,void (*destructor)(void*));
/**
 * Construcs a new `Vec` with the specefied capacity.
 */
Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT,void (*destructor)(void*));

/**
 * Drops the `Vec` freeing all the resources held by the vector and its elements.
 * 
 * * Note its the user's responsiblity to free the `self` pointer. (only if it's heap allocated)
 */
void drop_vec(Self self);

/**
 * Pushes an element to the back of the `Vec`.
 * 
 * * `element` is moved afterwords.
 * * It's the user's responsiblity to free `element`. (only if it's heap allocated)
 */
void vec_push(Self self,void* element);

/**
 * Pops the last element of the `Vec`
 * 
 * * The element is moved afterwords.
 * * It's the user's responsiblity to drop `element`.
 */
void* vec_pop(Self self);

/**
 * Reserves capacity for at least `additional` more elements to be inserted in the given `Vec`.
 * The collection may reserve more space to speculatively avoid frequent reallocations.
 * After calling reserve, capacity will be greater than or equal to `len` + `additional`.
 * Does nothing if capacity is already sufficient.
 * 
 * ## Panics
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
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
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
 */
void vec_reserve_exact(Self self,usize additional);

/**
 * Moves all the elements of other into `self`, leaving `other` empty.
 * 
 * ## Panics
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
 */
void vec_append(Self self,Vec* other);

/**
 * Clones and appends all elements in `data` to the `Vec`.
 * 
 * Iterates over the `data`, clones each element, and then appends it to this `Vec`.
 * The `data` is traversed in-order.
 */
void vec_extend(Self self,void* data,usize len);

/**
 * Clears the vector, removing all values.
 * * Note that this method has no effect on the allocated memory or capacity of the vector.
 * * The destructor can simply be `NULL` if `T` doesn't hold any resource.
 */
void vec_clear(Self self);

/**
 * Inserts an `element` at position `index` within the vector, shifting all elements after it to the right.
 * 
 * ## Panics
 * Panics if `index > len`.
 */
void vec_insert(Self self,usize index,void* element);

/**
 * Removes and returns the element at position `index` within the vector, shifting all elements after it to the left.
 * 
 * * Note: Because this shifts over the remaining elements, it has a worst-case performance of `O(n)`.
 * If you don't need the order of elements to be preserved, use `vec_swap_remove` instead.
 * * It's the users responsiblity to drop `element`.
 * 
 * ## Panics
 * Panics if index is out of bounds.
 */
void* vec_remove(Self self,usize index);

/**
 * Shortens the vector, keeping the first `len` elements and dropping the rest.
 * 
 * If `len` is greater or equal to the vector's current length, this has no effect.
 * 
 * The `vec_drain` method can emulate truncate, but causes the excess elements to be returned instead of dropped.
 * 
 * * Note that this method has no effect on the allocated capacity of the vector.
 */
void vec_truncate(Self self,usize len);

/**
 * Resizes the `Vec` in-place so that `len` is equal to `new_len`.
 * 
 * If `new_len` is greater than `len`, the `Vec` is extended by the difference,
 * with each additional slot filled with value.
 * If `new_len` is less than `len`, the `Vec` is simply truncated.
 * 
 * This method shallow-copies `value` byte-by-byte.
 * If you need more flexibility use `vec_resize_with`.
 * If you only need to resize to a smaller size, use `vec_truncate`.
 * 
 * * Note that freeing `value` is the user's responsiblity.
 * 
 * ## Safety
 * * Note that if `value` holds any resources, it may be freed more than once so be careful or prefer using `vec_resize_with`.
 */
void vec_resize(Self self,usize new_len,void* value);


#ifdef _cplusplus
}
#endif
#endif