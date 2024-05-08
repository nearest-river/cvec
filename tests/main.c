#include <stdio.h>
#include "../src/lib.h"

int main(int argc,const char** argv) {
  Vec vec=new_vec(sizeof(int));

  for(int i=0;i<32;i++) {
    vec_push(&vec,&i);
  }

  vec_reserve(&vec,10);

  // int* popped=vec_pop(&vec);
  // printf("popped: %d\n",*popped);

  int* arr=vec.ptr;
  for(int i=0;i<vec.len;i++) {
    printf("%d, ",arr[i]);
  }
  printf("\n");


  return 0;
}

