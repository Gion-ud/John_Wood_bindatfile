all: main datinfo

main: main.c
	cc -O2 -s $^ -o $@ -I../include -I../win-include -L../lib -lmman -lz

datinfo: datinfo.c
	cc -O2 -s $^ -o $@ -I../include -I../win-include -L../lib -lmman -lz