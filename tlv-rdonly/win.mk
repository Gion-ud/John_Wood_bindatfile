all: main rdat

main: main.c
	cc -O2 -s $^ -o $@ -I../include -I../win-include -L../lib -lmman -lz

rdat: rdat.c
	cc -O2 -s $^ -o $@ -I../include -I../win-include -L../lib -lmman -lz
