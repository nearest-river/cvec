#include <stdio.h>
#include "../src/lib.h"

void print_vec(Vec* vec);


int main(int argc,const char** argv) {
  Vec vec=new_vec(sizeof(int),NULL);
  Vec vec1=new_vec(sizeof(int),NULL);

  for(int i=0;i<32;i++) {
    vec_push(&vec,&i);
    vec_push(&vec1,&i);
  }

  vec_extend(&vec,vec1.ptr,vec1.len);


  int* popped=vec_pop(&vec);
  printf("popped: %d\n",*popped);

  int xd=69;
  vec_insert(&vec,0,&xd);
  print_vec(&vec);

  vec_resize(&vec,32,NULL);
  print_vec(&vec);

  return 0;
}

void print_vec(Vec* vec) {
  Vec this=*vec;

  int* arr=this.ptr;
  for(int i=0;i<this.len;i++) {
    printf("%d, ",arr[i]);
  }
  printf("\n");
}


