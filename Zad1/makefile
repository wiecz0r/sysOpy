GCC = gcc -Wall

all: static shared

static:
	$(GCC) -c array.c
	ar rcs array.a array.o
	make clean

shared:
	$(GCC) -fPIC -c array.c
	$(GCC) -fPIC -shared array.o -o array.so
	make clean

clean:
	rm -f *.o
