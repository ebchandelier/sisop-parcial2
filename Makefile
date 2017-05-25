all: barbeiro nossoExemplo

barbeiro: 
	gcc -c src/cthread.c exemplos/barbeiro.c -Wall
	gcc -o bin/barbeiroExe bin/support.o cthread.o barbeiro.o
	rm *.o
	./bin/barbeiroExe  

nossoExemplo:
	gcc -c src/cthread.c exemplos/nossoExemplo.c -Wall
	gcc -o bin/nossoExemploExe bin/support.o cthread.o nossoExemplo.o
	rm *.o
	./bin/nossoExemploExe  

