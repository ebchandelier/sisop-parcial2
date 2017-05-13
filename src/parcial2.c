#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

ucontext_t contextFibo, contextTRI, contextPA, contextPG, contextStartLoop;
FILA2 fila;

int runFibo(int n) {

	if(n==0) {

		return 0;

	} else if(n==1) {

		return 1;

	} else {

		return runFibo(n-1) + runFibo(n-2);
	}

	return -666;
}
 
void Fibo() {

	int n = 1;
	while(n<=12) {
	
		printf("FIBO termo %d : %d\n", n, runFibo(n));
		n++;
		
		if(n<=12) {

			AppendFila2(&fila, (void*)&contextFibo);
		}		
		
		swapcontext(&contextFibo, &contextStartLoop);
	}
}



int runTRI(int n) {

	if(n==0) {

		return 1;

	} else {

		return runTRI(n-1) + n-1;
	}

	return -666;
}


void TRI() {

	int n = 1;
	while(n<=6) {
	
		printf("TRI termo %d : %d\n", n, runTRI(n));
		n++;
		
		if(n<=6) {

			AppendFila2(&fila, (void*)&contextTRI);
		}		
		
		swapcontext(&contextTRI, &contextStartLoop);
	}
}


int runPA(int n) {
	
	if(n==0) {

		return 1;

	} else {

		return 4 + runPA(n-1);
	}

	return -666;
}

void PA() {

	int n = 1;
	while(n<=8) {
	
		printf("PA termo %d : %d\n", n, runPA(n));
		n++;
		
		if(n<=8) {

			AppendFila2(&fila, (void*)&contextPA);
		}		
		
		swapcontext(&contextPA, &contextStartLoop);
	}
}
	

int runPG(int n) {

	if(n==1) {

		return 1;

	} else {

		return 2 * runPG(n-1);
	}

	return -666;
}

void PG() {

	int n = 1;
	while(n<=10) {
	
		printf("PG termo %d : %d\n", n, runPG(n));
		n++;
		
		if(n<=10) {

			AppendFila2(&fila, (void*)&contextPG);
		}		
		
		swapcontext(&contextPG, &contextStartLoop);
	}
}


int main(int argc, char const *argv[])
{
	//printf("%d %d %d %d\n", Fibo(10), TRI(10), PA(10), PG(10));
	
	
	char stackFibo[8192];
	getcontext(&contextFibo);
    contextFibo.uc_stack.ss_sp = stackFibo;
    contextFibo.uc_stack.ss_size = sizeof stackFibo;
    contextFibo.uc_link = &contextFibo;
    makecontext(&contextFibo, (void*)&Fibo, 0);

    char stackTRI[8192];
	getcontext(&contextTRI);
    contextTRI.uc_stack.ss_sp = stackTRI;
    contextTRI.uc_stack.ss_size = sizeof stackTRI;
    contextTRI.uc_link = &contextTRI;
    makecontext(&contextTRI, (void*)&TRI, 0);

    char stackPA[8192];
	getcontext(&contextPA);
    contextPA.uc_stack.ss_sp = stackPA;
    contextPA.uc_stack.ss_size = sizeof stackPA;
    contextPA.uc_link = &contextPA;
    makecontext(&contextPA, (void*)&PA, 0);

    char stackPG[8192];
	getcontext(&contextPG);
    contextPG.uc_stack.ss_sp = stackPG;
    contextPG.uc_stack.ss_size = sizeof stackPG;
    contextPG.uc_link = &contextPG;
    makecontext(&contextPG, (void*)&PG, 0);

    
    AppendFila2(&fila, (void*)&contextPA);
    AppendFila2(&fila, (void*)&contextPG);
    AppendFila2(&fila, (void*)&contextFibo);
    AppendFila2(&fila, (void*)&contextTRI);
    
/*
    TCB_t fiboThread, triThread, paThread, pgThread;
    fiboThread.context = contextFibo;
    triThread.context = contextTRI;
    paThread.context = contextPA;
    pgThread.context = contextPG;
*/
    
    getcontext(&contextStartLoop);
    printf("passou por aqui  ");

    if(FirstFila2(&fila) == 0) {

	    ucontext_t* context = (ucontext_t*)GetAtIteratorFila2(&fila);
		DeleteAtIteratorFila2(&fila);

		//makecontext(context, (void*))

		setcontext(context);

    }
    
   	printf("Fim\n");
   	return 0;

//PA termo i : a
//PG termo i : b
//Fibo termo i: f
//Tri termo i : t 



    //getcontext(&contextTest);
    //setcontext(&contextFibo);

	return 0;
}