all:
	gcc -Wall -lpthread -lm -pthread -O2 main.c -o main > /dev/null
	./main 5 mountain.pgm filter2.txt out.pgm > /dev/null 2>&1
	./main 8 mountain.pgm filter2.txt out.pgm > /dev/null 2>&1
	./main 12 mountain.pgm filter2.txt out.pgm > /dev/null 2>&1

clear:
	rm -f main Times.txt out.pgm
fresh:
	rm -f main
