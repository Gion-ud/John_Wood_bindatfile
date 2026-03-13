all: bindatfile.o mkdatfile datinfo

ZLIB_PATH = /John_Wood/Coding_Projects/c/zlib

bindatfile.o: bindatfile/bindatfile.c
	cc -O2 -c -Iinclude -I$(ZLIB_PATH)/include -I. $< -o $@ -Wall -Wextra -Werror


mkdatfile: mkdatfile.c bindatfile.o
	cc -O2 -Iinclude $^ -o $@ -Wall -Wextra -Werror -L$(ZLIB_PATH) -lzdll

datinfo: datinfo.c bindatfile.o
	cc -O2 -Iinclude $^ -o $@ -Wall -Wextra -Werror -L$(ZLIB_PATH) -lzdll

clean:
	rm *.exe *.o
