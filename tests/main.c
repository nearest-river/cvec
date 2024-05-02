#include <stdio.h>
#include "../src/lib.h"

int main(int argc,const char** argv) {
  Vec vec=new_vec(sizeof(int));

  for(int i=0;i<16;i++) {
    vec_push(&vec,&i);
  }

  int* arr=vec.ptr;
  for(int i=0;i<16;i++) {
    printf("%d\n",arr[i]);
  }


  return 0;
}

