all: main rdat

main: main.c
	cc -O2 -s $^ -o $@ -I../include -lz

rdat: rdat.c
	cc -O2 -s $^ -o $@ -I../include -lz
