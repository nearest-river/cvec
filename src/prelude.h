#include <stdio.h>
#include <stdlib.h>

#define NULL ((void*)0)


typedef __SIZE_TYPE__ usize;

typedef struct Vec {
  void* ptr;
  usize BYTES_PER_ELEMENT;
  usize len;
  usize capacity;
} Vec;

