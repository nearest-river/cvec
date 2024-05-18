#ifndef VEC_LIB_H
#define VEC_LIB_H

#ifdef _cplusplus
extern "C" {
#endif
#ifndef _cplusplus
#include <stdbool.h>
#endif

#define MAX_CAPACITY 0x7fffffffUL
#define DEFAULT_CAPACITY 16


typedef __SIZE_TYPE__ usize;

/**
 * A function that frees the resources held by `self`.
 */
typedef void (*Destructor)(void*);

/**
 * A function that clones from `src` to `dest` without forgetting about the resources held by `self`.
 * 
 * * Params: `(void* dest,void* src)`.
 */
typedef void (*Cloner)(void*,void*);

/**
 * This virtual table keeping track of the resources held by the `Vec`.
 */
typedef struct VecVTable {
  Destructor destructor;
  Cloner cloner;
} VecVTable;

/**
 * `Vec` stands for `vector`
 */
typedef struct Vec {
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
  VecVTable vtable;
  void* ptr;
} Vec;

/**
 * The `Slice` type.
 * It holds a reference to some contigious data somewhere in the memory.
 */
typedef struct Slice {
  usize BYTES_PER_ELEMENT;
  usize len;
  void* data;
} Slice;



/**
 * Constructs a new empty `Vec` with default a capacity.
 */
Vec new_vec(usize BYTES_PER_ELEMENT,VecVTable vtable);
/**
 * Construcs a new `Vec` with the specefied capacity.
 */
Vec new_vec_with_capacity(usize capacity,usize BYTES_PER_ELEMENT,VecVTable vtable);

/**
 * Drops the `Vec` freeing all the resources held by the vector and its elements.
 * 
 * * Note its the user's responsiblity to free the `self` pointer. (only if it's heap allocated)
 */
void drop_vec(Vec* self);

/**
 * Pushes an element to the back of the `Vec`.
 * 
 * * `element` is moved afterwords.
 * * It's the user's responsiblity to free `element`. (only if it's heap allocated)
 */
void vec_push(Vec* self,void* element);

/**
 * Pops the last element of the `Vec`
 * 
 * * The element is moved afterwords.
 * * It's the user's responsiblity to drop `element`.
 */
void* vec_pop(Vec* self);

/**
 * Reserves capacity for at least `additional` more elements to be inserted in the given `Vec`.
 * The collection may reserve more space to speculatively avoid frequent reallocations.
 * After calling reserve, capacity will be greater than or equal to `len` + `additional`.
 * Does nothing if capacity is already sufficient.
 * 
 * ## Panics
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
 */
void vec_reserve(Vec* self,usize additional);

/**
 * Reserves the minimum capacity for at least additional more elements to be inserted in the given `Vec`.
 * Unlike reserve, this will not deliberately over-allocate to speculatively avoid frequent allocations.
 * After calling `vec_reserve_exact`, capacity will be greater than or equal to self->len + additional.
 * Does nothing if the capacity is already sufficient.
 * 
 * * Note that the allocator may give the collection more space than it requests.
 * Therefore, capacity can not be relied upon to be precisely minimal.
 * Prefer `vec_reserve` if future insertions are expected.
 * 
 * ## Panics
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
 */
void vec_reserve_exact(Vec* self,usize additional);

/**
 * Moves all the elements of other into `self`, leaving `other` empty.
 * 
 * ## Panics
 * Panics if the new capacity exceeds `MAX_CAPACITY` bytes.
 */
void vec_append(Vec* self,Vec* other);

/**
 * Clones and appends all elements in `data` to the `Vec`.
 * 
 * Iterates over the `data`, clones each element, and then appends it to this `Vec`.
 * The `data` is traversed in-order.
 */
void vec_extend(Vec* self,void* data,usize len);

/**
 * Clears the vector, removing all values.
 * * Note that this method has no effect on the allocated memory or capacity of the vector.
 * * The destructor can simply be `NULL` if `T` doesn't hold any resource.
 */
void vec_clear(Vec* self);

/**
 * Inserts an `element` at position `index` within the vector, shifting all elements after it to the right.
 * 
 * ## Panics
 * Panics if `index > len`.
 */
void vec_insert(Vec* self,usize index,void* element);

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
void* vec_remove(Vec* self,usize index);

/**
 * Shortens the vector, keeping the first `len` elements and dropping the rest.
 * 
 * If `len` is greater or equal to the vector's current length, this has no effect.
 * 
 * The `vec_drain` method can emulate truncate, but causes the excess elements to be returned instead of dropped.
 * 
 * * Note that this method has no effect on the allocated capacity of the vector.
 */
void vec_truncate(Vec* self,usize len);

/**
 * Resizes the `Vec` in-place so that `len` is equal to `new_len`.
 * 
 * If `new_len` is greater than `len`, the `Vec` is extended by the difference,
 * with each additional slot filled with value.
 * If `new_len` is less than `len`, the `Vec` is simply truncated.
 * 
 * * This method clones `value` with the cloner function.
 * * If you need more flexibility use `vec_resize_with`.
 * * If you only need to resize to a smaller size, use `vec_truncate`.
 * 
 * * Note that freeing the `value` pointer is the user's responsiblity. (only if it is allocated on the heap)
 */
void vec_resize(Vec* self,usize new_len,void* value);

/**
 * Resizes the `Vec` in-place so that `len` is equal to `new_len`.
 * 
 * If `new_len` is greater than `len`, the `Vec` is extended by the difference,
 * with each additional slot filled with the result of calling the function `f`.
 * The return values from `f` will end up in the `Vec` in the order they have been generated.
 * 
 * If `new_len` is less than `len`, the `Vec` is simply truncated.
 * 
 * This method uses a function to create new values on every push.
 * If you'd rather `Clone` a given value, use `vec_resize`.
 * If you want to use the default values trait to generate values, you can pass `default` as the second argument.
 */
void vec_resize_with(Vec* self,usize new_len,void* (*f)(void));

/**
 * Retains only the elements specified by the predicate.
 * 
 * In other words, remove all elements e for which `f(&element)` returns `false`.
 * This method operates in place,
 * visiting each element exactly once in the original order,
 * and preserves the order of the retained elements.
 */
void vec_retain(Vec* self,bool (*f)(void*));

/**
 * Shrinks the capacity of the vector with a lower bound.
 * The capacity will remain at least as large as both the length and the supplied value.
 * If the current capacity is less than the lower limit, this is a no-op.
 */
void vec_shrink_to(Vec* self,usize min_capacity);

/**
 * Shrinks the capacity of the vector to its length, so `self->len==self->capacity`.
 */
void vec_shrink_to_fit(Vec* self);

/**
 * Returns the remaining spare capacity of the vector as a slice.
 * The returned slice can be used to fill the vector with data.
 * (e.g. by reading from a file) before marking the data as initialized.
 */
Slice vec_spare_capacity(Vec* self);

/**
 * Removes an element from the vector and returns it.
 * The removed element is replaced by the last element of the vector.
 * This does not preserve ordering, but is O(1).
 * If you need to preserve the element order, use `vec_remove` instead.
 * 
 * ## Panics
 * * Panics if index is out of bounds.
 */
void* vec_swap_remove(Vec* self,usize index);

#ifdef _cplusplus
}
#endif
#endif
