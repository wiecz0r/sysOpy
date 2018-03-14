GCC = gcc -Wall
M_PATH = main

all: O O0 O1 O2 O3 Os test clean

compile:
	$(GCC) -c array.c $(OPT)
	ar rcs libarray.a array.o
	$(GCC) -fPIC -c array.c $(OPT)
	$(GCC) -fPIC -shared array.o -o libarray.so $(OPT)

O:
	make compile OPT=-O
	$(GCC) -O main.c libarray.a -o $(M_PATH)_static_O
	#$(GCC) -O main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_O
	$(GCC) -O main.c -L. -larray -o $(M_PATH)_shared_O
	$(GCC) -O main.c -ldl -o $(M_PATH)_dynamic_O -D DLL

O0:
	make compile OPT=-O0
	$(GCC) -O0 main.c libarray.a -o $(M_PATH)_static_O0
	#$(GCC) -O main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_O0
	$(GCC) -O0 main.c -L. -larray -o $(M_PATH)_shared_O0
	$(GCC) -O0 main.c -ldl -o $(M_PATH)_dynamic_O0 -D DLL

O1:
	make compile OPT=-O1
	$(GCC) -O1 main.c libarray.a -o $(M_PATH)_static_O1
	#$(GCC) -O1 main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_O1
	$(GCC) -O1 main.c -L. -larray -o $(M_PATH)_shared_O1
	$(GCC) -O1 main.c -ldl -o $(M_PATH)_dynamic_O1 -D DLL

O2:
	make compile OPT=-O2
	$(GCC) -O2 main.c libarray.a -o $(M_PATH)_static_O2
	#$(GCC) -O2 main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_O2
	$(GCC) -O2 main.c -L. -larray -o $(M_PATH)_shared_O2
	$(GCC) -O2 main.c -ldl -o $(M_PATH)_dynamic_O2 -D DLL

O3:
	make compile OPT=-O3
	$(GCC) -O3 main.c libarray.a -o $(M_PATH)_static_O3
	#$(GCC) -O3 main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_O3
	$(GCC) -O3 main.c -L. -larray -o $(M_PATH)_shared_O3
	$(GCC) -O3 main.c -ldl -o $(M_PATH)_dynamic_O3 -D DLL

Os:
	make compile OPT=-Os
	$(GCC) -Os main.c libarray.a -o $(M_PATH)_static_Os
	#$(GCC) -Os main.c -L. -Wl,-rpath=. -larray -o $(M_PATH)_shared_Os
	$(GCC) -Os main.c -L. -larray -o $(M_PATH)_shared_Os
	$(GCC) -Os main.c -ldl -o $(M_PATH)_dynamic_Os -D DLL




clean:
	rm -f *.o *.a *.so main_*

fresh: clean 
	rm -f *.txt

test:
	echo "STATIC\n\n" > results3b.txt
	./$(M_PATH)_static_O 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static_O0 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static_O1 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static_O2 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static_O3 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_static_Os 2000 100 dynamic da 300 sda 300
	echo "\nSHARED\n\n" >> results3b.txt
	./$(M_PATH)_shared_O 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared_O0 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared_O1 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared_O2 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared_O3 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_shared_Os 2000 100 dynamic da 300 sda 300
	echo "\nDYNAMIC\n\n" >> results3b.txt
	./$(M_PATH)_dynamic_O 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic_O0 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic_O1 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic_O2 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic_O3 2000 100 dynamic da 300 sda 300
	./$(M_PATH)_dynamic_Os 2000 100 dynamic da 300 sda 300
