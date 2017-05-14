#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

ucontext_t contextFibo, contextTRI, contextPA, contextPG, contextStartLoop;
TCB_t fiboThread, triThread, paThread, pgThread;
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

			AppendFila2(&fila, (void*)&fiboThread);
		}		
		
		swapcontext(&(fiboThread.context), &contextStartLoop);
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

			AppendFila2(&fila, (void*)&triThread);
		}		
		
		swapcontext(&(triThread.context), &contextStartLoop);
	}
}


int runPA(int n) {
	
	if(n==1) {

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

			AppendFila2(&fila, (void*)&paThread);
		}		
		
		swapcontext(&(paThread.context), &contextStartLoop);
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

			AppendFila2(&fila, (void*)&pgThread);
		}		
		
		swapcontext(&(pgThread.context), &contextStartLoop);
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

    
    fiboThread.context = contextFibo;
    triThread.context = contextTRI;
    paThread.context = contextPA;
    pgThread.context = contextPG;

	
	AppendFila2(&fila, (void*)&paThread);
    AppendFila2(&fila, (void*)&pgThread);
    AppendFila2(&fila, (void*)&fiboThread);
    AppendFila2(&fila, (void*)&triThread);
    
  	//printf("main loop");
    getcontext(&contextStartLoop);
    
    if(FirstFila2(&fila) == 0) {

	    TCB_t* tcb = (TCB_t*)GetAtIteratorFila2(&fila);
		DeleteAtIteratorFila2(&fila);

		setcontext(&(*tcb).context);
	}
    
   	//printf("Fim\n");
   	return 0;
}