
LIB_NAME=cvec
CFLAGS=-Wall -g

test:
	./.build cvec && gcc $(CFLAGS) ./tests/main.c -L ./include -lcvec -o ./bin/test && ./bin/test

build:
	./.build cvec


