P = program
F = data.txt

all: compile test

compile:
	gcc -Wall main.c -o program

clean: 
	rm -f *.txt* program

test:
	./$(P) generate $(F) 500000 4
	./$(P) copy $(F) $(F).v2 500000 4 sys
	./$(P) copy $(F) $(F).v2 500000 4 lib
	./$(P) sort $(F) 4000 4 sys
	./$(P) sort $(F).v2 4000 4 lib

	./$(P) generate $(F) 200000 4
	./$(P) copy $(F) $(F).v2 200000 4 sys
	./$(P) copy $(F) $(F).v2 200000 4 lib
	./$(P) sort $(F) 3000 4 sys
	./$(P) sort $(F).v2 3000 4 lib

	./$(P) generate $(F) 100000 512
	./$(P) copy $(F) $(F).v2 100000 512 sys
	./$(P) copy $(F) $(F).v2 100000 512 lib
	./$(P) sort $(F) 4000 512 sys
	./$(P) sort $(F).v2 4000 512 lib

	./$(P) generate $(F) 80000 512
	./$(P) copy $(F) $(F).v2 80000 512 sys
	./$(P) copy $(F) $(F).v2 80000 512 lib
	./$(P) sort $(F) 3000 512 sys
	./$(P) sort $(F).v2 3000 512 lib
