#include "prelude.h"

void* alloc(usize size) {
  void* ptr=malloc(size);

  if(!ptr) {
    printf("Could'nt allocate memory.\nmalloc returned `NULL` pointer");
    exit(1);
  }

  return ptr;
}

