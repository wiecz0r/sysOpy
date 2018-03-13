GCC = gcc -Wall
M_PATH = main

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

fresh:
	make clean
	make clean_file
	rm -f *.a *.so main

trivial:
	$(GCC) main.c array.c -o main
	make clean

test:
	./$(M_PATH) 2000 100 dynamic da 300 sda 300
	./$(M_PATH) 2000 100 static da 300 sda 300
	./$(M_PATH) 2000 100 dynamic ascii 300 da 200
	./$(M_PATH) 2000 100 static ascii 300 da 200
	./$(M_PATH) 2000 100 dynamic sda 700 ascii 400
	./$(M_PATH) 2000 100 static sda 700 ascii 400
	./$(M_PATH) 4000 50 dynamic da 300 sda 300
	./$(M_PATH) 4000 50 static da 300 sda 300
	./$(M_PATH) 3000 100 dynamic ascii 300 da 200
	./$(M_PATH) 3000 100 static ascii 300 da 200
	./$(M_PATH) 1000 200 dynamic sda 700 ascii 400
	./$(M_PATH) 1000 200 static sda 700 ascii 400

clean_file:
	rm -f *.txt