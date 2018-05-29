filename = totem.pgm
all:
	gcc -Wall -lpthread -lm -pthread -O2 main.c -o main > /dev/null
	./main 1 $(filename) filter1.txt out1.pgm > /dev/null
	./main 2 $(filename) filter1.txt out1.pgm > /dev/null
	./main 4 $(filename) filter1.txt out1.pgm > /dev/null
	./main 8 $(filename) filter1.txt out1.pgm > /dev/null
	./main 1 $(filename) filter2.txt out2.pgm > /dev/null
	./main 2 $(filename) filter2.txt out2.pgm > /dev/null
	./main 4 $(filename) filter2.txt out2.pgm > /dev/null
	./main 8 $(filename) filter2.txt out2.pgm > /dev/null
	./main 1 $(filename) filter3.txt out3.pgm > /dev/null
	./main 2 $(filename) filter3.txt out3.pgm > /dev/null
	./main 4 $(filename) filter3.txt out3.pgm > /dev/null
	./main 8 $(filename) filter3.txt out3.pgm > /dev/null

clear:
	rm -f main Times.txt out*.pgm
fresh:
	rm -f main out*.pgm
