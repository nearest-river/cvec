#include <stdio.h>
#include "../src/lib.h"
#include <stdlib.h>

void test_bytes();
bool is_even(void* ptr);

int main() {
  test_bytes();
  return 0;
}



void test_bytes() {
  Vec vec=new_vec_with_capacity(MAX_CAPACITY,sizeof(unsigned char),NULL);

  for(char c='A';c<='z';c++) {
    if(c-1=='Z') c='a';
    vec_push(&vec,&c);
  }

  vec_retain(&vec,is_even);

  for(usize i=0;i<vec.len;i++) {
    printf("%c, ",((unsigned char*)vec.ptr)[i]);
  }
  printf("len: %ld\n",vec.len);
}

bool is_even(void* ptr) {
  return *((int*)ptr)%2==0;
}

