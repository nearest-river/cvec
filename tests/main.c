#include "../lib/lib.h"
#include <stdio.h>



int main(int argc,const char** argv) {
  const VecVTable vtable={
    .cloner=NULL,
    .destructor=NULL
  };
  Vec vec=vec_new(sizeof(int),vtable);

  for(int i=0;i<10;i++) {
    vec_push(&vec,&i);
  }

  int* slice=vec.ptr;
  for(usize i=0;i<vec.len;i++) {
    printf("%d ",*slice++);
  }

  printf("\n");

  return 0;
}



