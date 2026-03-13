all: test

test: test.c hash_index.c
	cc -O2 -s -I../include $^ -o $@ -Wall -Wextra -Werror
