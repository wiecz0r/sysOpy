P = program
F = wynik.txt

all: compile test fresh

compile:
	gcc -Wall normal_dir.c -o $(P) -std=c99 -g -ggdb

clean: fresh
	rm -f $(F)

test:
	./$(P) ../ '>' 2017 12 12 12 01 23 > $(F)
	./$(P) ../ '>' 2017 12 12 12 01 23

fresh: 
	rm -f $(P)
