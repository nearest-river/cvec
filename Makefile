
LIB_NAME=cvec
CFLAGS=-Wall -g


test:
	./.build $(LIB_NAME) && gcc $(CFLAGS) ./tests/main.c -L ./include -l$(LIB_NAME) -o ./bin/test && ./bin/test

build:
	./.build $(LIB_NAME)


