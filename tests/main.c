#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/lib.h"

#define DUMMY_TXT "lol..xd"
#define DUMMY_TXT_LEN 7

Vec read_file(FILE* file);
void drop_file(void*);

void test_files();
void test_buf();


int main(int argc,const char** argv) {
  test_files();
  return 0;
}


void test_buf() {
  FILE* file=fopen("./target/tmp/xt.txt","r");
  if(NULL==file) {
    perror("File not found.");
    exit(1);
  }

  Vec buf=vec_with_capacity(120,sizeof(char),(VecVTable){NULL,NULL});

  for(char c=fgetc(file);EOF!=c;c=fgetc(file)) {
    vec_push(&buf,&c);
  }
  char* str=buf.ptr;

  puts(str);
  printf("len: %ld\n",buf.len);
}

void test_files() {
  VecVTable vtable={
    .cloner=NULL,
    .destructor=drop_file
  };
  Vec vec=vec_new(sizeof(FILE*),vtable);
  char path[]="./target/tmp/xd0.txt";

  for(int i=0;i<10;i++) {
    path[0xF]=i+48;
    FILE* f=fopen(path,"w+");

    fprintf(f,DUMMY_TXT);
    fclose(f);

    FILE* file=fopen(path,"r");
    vec_push(&vec,&file);
  }

  FILE** files=vec.ptr;
  for(int i=0;i<vec.len;i++) {
    Vec buf=read_file(files[i]);

    printf("file %d: %s, len: %ld\n",i,(char*)buf.ptr,buf.len);
    drop_vec(&buf);
  }

  // free(buf);
  drop_vec(&vec);
}

Vec read_file(FILE* file) {
  Vec buf=vec_new(sizeof(char),(VecVTable){NULL,NULL});

  for(char c=fgetc(file);EOF!=c;c=fgetc(file)) {
    vec_push(&buf,&c);
  }

  return buf;
}

void drop_file(void* file) {
  fclose(*((FILE**)file));
}

