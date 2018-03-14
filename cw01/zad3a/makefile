GCC = gcc -Wall
M_PATH = main

all: static shared dynamic clean test

static:
	$(GCC) -c array.c
	ar rcs libarray.a array.o
	$(GCC) main.c -L. -larray -o $(M_PATH)_static
	make clean

shared:
	$(GCC) -fPIC -c array.c
	$(GCC) -fPIC -shared array.o -o libarray.so
	#$(GCC) main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared
	$(GCC) main.c -L. -larray -o $(M_PATH)_shared
	make clean

dynamic:
	$(GCC) -fPIC -c array.c
	$(GCC) -fPIC -shared array.o -o libarray.so
	$(GCC) main.c -ldl -o $(M_PATH)_dynamic -D DLL
	make clean

clean:
	rm -f *.o

fresh: clean 
	rm -f *.a *.so main_* 

test:
	./$(M_PATH)_static 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static 2000 100 static da 300 sda 300
	./$(M_PATH)_static 4000 50 dynamic da 300 sda 300
	./$(M_PATH)_static 4000 50 static da 300 sda 300
	./$(M_PATH)_static 3000 100 dynamic ascii 300 da 200
	./$(M_PATH)_static 3000 100 static ascii 300 da 200
	./$(M_PATH)_static 1000 200 dynamic sda 700 ascii 400
	./$(M_PATH)_static 1000 200 static sda 700 ascii 400
	#
	./$(M_PATH)_shared 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared 2000 100 static da 300 sda 300
	./$(M_PATH)_shared 4000 50 dynamic da 300 sda 300
	./$(M_PATH)_shared 4000 50 static da 300 sda 300
	./$(M_PATH)_shared 3000 100 dynamic ascii 300 da 200
	./$(M_PATH)_shared 3000 100 static ascii 300 da 200
	./$(M_PATH)_shared 1000 200 dynamic sda 700 ascii 400
	./$(M_PATH)_shared 1000 200 static sda 700 ascii 400
	#
	./$(M_PATH)_dynamic 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic 2000 100 static da 300 sda 300
	./$(M_PATH)_dynamic 4000 50 dynamic da 300 sda 300
	./$(M_PATH)_dynamic 4000 50 static da 300 sda 300
	./$(M_PATH)_dynamic 3000 100 dynamic ascii 300 da 200
	./$(M_PATH)_dynamic 3000 100 static ascii 300 da 200
	./$(M_PATH)_dynamic 1000 200 dynamic sda 700 ascii 400
	./$(M_PATH)_dynamic 1000 200 static sda 700 ascii 400
