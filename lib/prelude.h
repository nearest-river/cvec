#ifndef VEC_PRELUDE_H
#define VEC_PRELUDE_H

#include "../include/cstd-def/src/lib.h"
#include "lib.h"


#define max(x,y) x>y?x:y
#define vec__index__ptr(__self__,__index__) __self__->ptr+(__self__->BYTES_PER_ELEMENT*__index__)




#endif
