#include <stdio.h>
#include "../src/lib.h"
#include <stdlib.h>

void print_vec(Vec* vec);
void fill_vec(Vec* vec);

int main(int argc,const char** argv) {
  Vec vec=new_vec_with_capacity(MAX_CAPACITY,sizeof(Vec),(void (*)(void*))drop_vec);

  for(int i=0;i<32;i++) {
    Vec inner_vec=new_vec(sizeof(int),NULL);

    fill_vec(&inner_vec);
    vec_push(&vec,&inner_vec);
  }

  Vec* arr=vec.ptr;
  for(usize i=0;i<vec.len;i++) {
    print_vec(arr++);
  }

  drop_vec(&vec);
  return 0;
}

void print_vec(Vec* vec) {
  Vec this=*vec;

  int* arr=this.ptr;
  for(usize i=0;i<this.len;i++) {
    printf("%d, ",arr[i]);
  }
  printf("\n");
}

void fill_vec(Vec* vec) {
  for(int i=0;i<32;i++) {
    vec_push(vec,&i);
  }
}


