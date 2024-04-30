#ifndef VEC_PRELUDE_H
#define VEC_PRELUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib.h"

#define max(x,y) x>y?x:y
#define MIN_NON_ZERO_CAP(BYTES_PER_ELEMENT) BYTES_PER_ELEMENT==1?8:BYTES_PER_ELEMENT<=1024?4:1

#define not_null(ptr) assert(ptr!=NULL)
#define not_null2(ptr1,ptr2) assert(ptr1!=NULL);assert(ptr2!=NULL)

#define bool unsigned char;
#define true 1
#define false 0


#endif