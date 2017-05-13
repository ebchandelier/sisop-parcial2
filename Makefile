all: 
	gcc -c src/parcial2.c -Wall
	gcc -o bin/Executavel bin/support.o parcial2.o
	rm *.o
	./bin/Executavel  
