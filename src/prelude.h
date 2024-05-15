#ifndef VEC_PRELUDE_H
#define VEC_PRELUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib.h"
#ifndef _cplusplus
#include <stdbool.h>
#endif


#define max(x,y) x>y?x:y
#define MIN_NON_ZERO_CAP(BYTES_PER_ELEMENT) BYTES_PER_ELEMENT==1?8:BYTES_PER_ELEMENT<=1024?4:1
#define vec__index(__vec__,__index__) ptr__index(__vec__.ptr,__vec__.BYTES_PER_ELEMENT,__index__)
#define ptr__index(__pointer_address__,__bytes_per_element__,__index__) __pointer_address__+(__bytes_per_element__*__index__)

#define not_null(ptr) assert(ptr)
#define not_null2(ptr1,ptr2) assert(ptr1);assert(ptr2)
#define panic(...) { fprintf(stderr,__VA_ARGS__);exit(1); }



#endif