#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/support.h"

#include <stdlib.h>
#include <stdio.h> 
#include <ucontext.h>
#include <string.h>
#include <ucontext.h>

#define STACKSIZE 8192

struct sJoinDependance {
	TCB_t *dependant;
	int dependsOn;
};

typedef struct sJoinDependance Dependance;

FILA2 aptos0, aptos1, aptos2, aptos3;
FILA2 executando;
FILA2 filaJoin;
FILA2 filaWait;

//TO-DO

//Review every findTid function.

int findTid(FILA2 *queue, int id){
	int invalid = FirstFila2(queue);
	TCB_t *element;

	while (!invalid){
		element = (TCB_t*)GetAtIteratorFila2(queue);

		if (element->tid == id)
			return 1;
		else
			invalid = NextFila2(queue);
	}
	return 0;
}

int findTidWait(FILA2 *queue, int id){

	int invalidSem = FirstFila2(queue);
	csem_t *element;
	FILA2 *subqueue;

	while (!invalidSem){
		element = (csem_t*)GetAtIteratorFila2(queue);
		subqueue = element->fila;

		if (findTid((subqueue), id))
			return 1;
		else
			invalidSem = NextFila2(queue);
	}

	return 0;
}

int findTidJoin(FILA2 *queue, int id){
	int invalid = FirstFila2(queue);
	Dependance *element;

	while (!invalid){
		element = (Dependance*)GetAtIteratorFila2(queue);

		if (element->dependant->tid == id)
			return 1;
		else
			invalid = NextFila2(queue);
	}
	return 0;
}

int findDependance(FILA2 *queue, int id){
	int invalid = FirstFila2(queue);
	Dependance *element;

	while (!invalid){
		element = (Dependance*)GetAtIteratorFila2(queue);

		if (element->dependsOn == id)
			return 1;
		else
			invalid = NextFila2(queue);
	}
	return 0;
}

int schedule(){

	if (!FirstFila2(&aptos0))
		return 0;

	if (!FirstFila2(&aptos1))
		return 1;

	if (!FirstFila2(&aptos2))
		return 2;

	if (!FirstFila2(&aptos3))
		return 3;

	return -1;
}

int exists(int tid){
	TCB_t *running;
	int isRunning = 0;
	if(!FirstFila2(&executando)){
		running = (TCB_t*)GetAtIteratorFila2(&executando);
		if (running->tid == tid)
			isRunning = 1;
	}


	return isRunning
		|| findTid(&aptos0, tid) || findTid(&aptos1, tid) || findTid(&aptos2, tid)
	 	|| findTid(&aptos3, tid) || findTidWait(&filaWait, tid) || findTidJoin(&filaJoin, tid);
	 	
}

//Check if any thread depends on current (returning) thread.
int checkDependances(int tid){

	if (findDependance(&filaJoin,tid)){
		//move from blocked to ready (according to its priority)
		TCB_t *newTCB;
		Dependance *dependance = (Dependance*)GetAtIteratorFila2(&filaJoin);
		newTCB = dependance->dependant;
		int prio = newTCB->ticket;

		if (DeleteAtIteratorFila2(&filaJoin)){
			return -1;
		}

		free(dependance);

		switch(prio){
			case 0:
				if (AppendFila2(&aptos0, (void*)newTCB)){
					return -1;
				}
				break;
			case 1:
				if (AppendFila2(&aptos1, (void*)newTCB)){
					return -1;
				}
				break;
			case 2:
				if (AppendFila2(&aptos2, (void*)newTCB)){
					return -1;
				}
				break;
			case 3:
				if (AppendFila2(&aptos3, (void*)newTCB)){
					return -1;
				}
				break;
			default:
				return -1;
				break;
		}


	}

	return 0;

}

void endThread(ucontext_t *newcontext){

	static int firstTime = 1;

	if(getcontext(newcontext)){
		printf("Erro criando contexto de retorno\n");
	}

	if (!firstTime){
		TCB_t *current;
		TCB_t *next;
		int nextThread;

		if (FirstFila2(&executando)){
			printf("Erro na funcao de retorno\n");
		}
		current = (TCB_t*)GetAtIteratorFila2(&executando);
		if (DeleteAtIteratorFila2(&executando)){
			printf("Erro[2] na funcao de retorno\n");
		}

		checkDependances(current->tid);


		//start

		nextThread = schedule();

		switch (nextThread){
			case 0:
				next = (TCB_t*)GetAtIteratorFila2(&aptos0);
				if (DeleteAtIteratorFila2(&aptos0)){
					printf("Erro[3,1] na funcao de retorno\n");
				}
				break;

			case 1:
				next = (TCB_t*)GetAtIteratorFila2(&aptos1);
				if (DeleteAtIteratorFila2(&aptos1)){
					printf("Erro[3,2] na funcao de retorno\n");
				}
				break;

			case 2:
				next = (TCB_t*)GetAtIteratorFila2(&aptos2);
				if (DeleteAtIteratorFila2(&aptos2)){
					printf("Erro[3,3] na funcao de retorno\n");
				}
				break;

			case 3:
				next = (TCB_t*)GetAtIteratorFila2(&aptos3);
				if (DeleteAtIteratorFila2(&aptos3)){
					printf("Erro[3,4] na funcao de retorno\n");
				}
				break;

			default:
				printf("Erro[3,5] na funcao de retorno\n");
				break;
		}

		if (AppendFila2(&executando, next)){
			printf("Erro[4] na funcao de retorno\n");
		}

		free(current);

		setcontext(next->context);

		//f
	}

	firstTime = 0;

}

//TO-DO
//Initialize return function's context
int ccreate (void* (*start)(void*), void *arg, int prio){
	static int firstTime = 1;
	static int tid_counter = 0;
	static ucontext_t returnFunction;

	if (prio < 0 || prio > 3){
		printf("Prioridade invalida\n");
		return -1;
	}

	if (firstTime){
		//initialize queues here, dumbass

		CreateFila2(&aptos0);
		CreateFila2(&aptos1);
		CreateFila2(&aptos2);
		CreateFila2(&aptos3);
		CreateFila2(&executando);
		CreateFila2(&filaJoin);
		CreateFila2(&filaWait);

		ucontext_t maincontext;

		if (getcontext(&maincontext)){
			printf("Erro criando contexto para a thread %d\n", tid_counter);
			return -1;
		}

		TCB_t *mainTCB = malloc(sizeof(TCB_t));

		mainTCB->tid = tid_counter++;
		mainTCB->state = 2;
		mainTCB->ticket = 0;
		mainTCB->context = maincontext;

		AppendFila2(&executando, (void*)mainTCB);

		firstTime = 0;

		// initialize returnFunction's context

		endThread(&returnFunction);

	}

	char *stackptr = malloc(STACKSIZE);
	ucontext_t newcontext;

	if(getcontext(&newcontext)){
		printf("Erro criando contexto para a thread %d\n", tid_counter);
		return -1;
	}

	newcontext.uc_stack.ss_sp = stackptr;
	newcontext.uc_stack.ss_size = STACKSIZE;
	newcontext.uc_link = &returnFunction;

	makecontext(&newcontext, (void*)start, 1, arg); //number of arguments = 1

	TCB_t *newTCB = malloc(sizeof(TCB_t));

	newTCB->tid = tid_counter++;
	newTCB->state = 1; //?
	newTCB->ticket = prio;
	newTCB->context = newcontext;

	switch(prio){
		case 0:
			AppendFila2(&aptos0, (void*)newTCB);
			break;
		case 1:
			AppendFila2(&aptos1, (void*)newTCB);
			break;
		case 2:
			AppendFila2(&aptos2, (void*)newTCB);
			break;
		case 3:
			AppendFila2(&aptos3, (void*)newTCB);
			break;
		default:
			break;

	}

	return tid_counter-1;
}

//TO-DO
//Review :)
int csetprio(int tid, int prio){
	TCB_t *element;
	int found = 0;
	
	// look for tid in filaWait

	if (findTidWait(&filaWait, tid)){

		csem_t *sem = (csem_t*)GetAtIteratorFila2(&filaWait);
		FILA2 *queue = sem->fila;

		element = (TCB_t*)GetAtIteratorFila2(queue);

		element->ticket = prio;

		return 0;
	}

	// look for tid in filaJoin

	if (findTidJoin(&filaJoin, tid)){
		Dependance *elementD = (Dependance*)GetAtIteratorFila2(&filaJoin);

		elementD->dependant->ticket = prio;

		return 0;
	}

	if (findTid(&executando, tid)){
		element = (TCB_t*)GetAtIteratorFila2(&executando);
		element->ticket = prio;
		return 0;
	}
	
	if (!found && findTid(&aptos0, tid)){
		found = 1;
		element = (TCB_t*)GetAtIteratorFila2(&aptos0);
		if (DeleteAtIteratorFila2(&aptos0))
			return -1;
	}

	if (!found && findTid(&aptos1, tid)){
		found = 1;
		element = (TCB_t*)GetAtIteratorFila2(&aptos1);
		if (DeleteAtIteratorFila2(&aptos1))
			return -1;
	}

	if (!found && findTid(&aptos2, tid)){
		found = 1;
		element = (TCB_t*)GetAtIteratorFila2(&aptos2);
		if (DeleteAtIteratorFila2(&aptos2))
			return -1;
	}

	if (!found && findTid(&aptos3, tid)){
		found = 1;
		element = (TCB_t*)GetAtIteratorFila2(&aptos3);
		if (DeleteAtIteratorFila2(&aptos3))
			return -1;
	}

	if (!found)
		return -1;

	element->ticket = prio;

	switch(prio){
		case 0:
			return AppendFila2(&aptos0, (void*)element);
			break;
		case 1:
			return AppendFila2(&aptos1, (void*)element);
			break;
		case 2:
			return AppendFila2(&aptos2, (void*)element);
			break;
		case 3:
			return AppendFila2(&aptos3, (void*)element);
			break;
		default:
			return -1;
			break;
	}
		
	return -1;
} 

int cyield(void){
	int currentPrio;
	int nextThread;
	TCB_t *current;
	TCB_t *next;
	ucontext_t *currentContext;
	ucontext_t *nextContext;

	if(FirstFila2(&executando))
		return -1;

	current = (TCB_t*)GetAtIteratorFila2(&executando);

	if (DeleteAtIteratorFila2(&executando))
		return -1;

	currentPrio = current->ticket;

	switch (currentPrio){
		case 0:
			AppendFila2(&aptos0, (void*) current);
			break;
		case 1:
			AppendFila2(&aptos1, (void*) current);
			break;
		case 2:
			AppendFila2(&aptos2, (void*) current);
			break;
		case 3:
			AppendFila2(&aptos3, (void*) current);
			break;
		default:
			return -1;
			break;
	}

	nextThread = schedule();

	switch (nextThread){
		case 0:
			next = (TCB_t*)GetAtIteratorFila2(&aptos0);
			if (DeleteAtIteratorFila2(&aptos0)){
				return -1;
			}
			break;

		case 1:
			next = (TCB_t*)GetAtIteratorFila2(&aptos1);
			if (DeleteAtIteratorFila2(&aptos1)){
				return -1;
			}
			break;

		case 2:
			next = (TCB_t*)GetAtIteratorFila2(&aptos2);
			if (DeleteAtIteratorFila2(&aptos2)){
				return -1;
			}
			break;

		case 3:
			next = (TCB_t*)GetAtIteratorFila2(&aptos3);
			if (DeleteAtIteratorFila2(&aptos3)){
				return -1;
			}
			break;

		default:
			return -1;
			break;
	}

	currentContext = &(current->context);
	nextContext = &(next->context);

	if (AppendFila2(&executando, next)){
		return -1;
	}

	swapcontext(currentContext,nextContext);

	return 0;
}

// TO-DO
// What if there's nothing else ready (is that even possible?)

int cjoin(int tid){
	int invalid = FirstFila2(&filaJoin);
	int nextThread;
	TCB_t *current;
	TCB_t *next;
	ucontext_t *currentContext;
	ucontext_t *nextContext;
	Dependance *element;

	// First we check if thread that we're waiting for exists
	if (!exists(tid))
		return -1;

	// Then we check if another thread is already waiting for a thread with tid
	while (!invalid){
		element = (Dependance*)GetAtIteratorFila2(&filaJoin);
		if (element->dependsOn == tid)
			return -1;

		invalid = NextFila2(&filaJoin);
	}

	// Everything is fine, we can put it on the join list and remove from running

	if (FirstFila2(&executando))
		return -1;

	current = (TCB_t*)GetAtIteratorFila2(&executando);

	if (DeleteAtIteratorFila2(&executando))
		return -1;

	Dependance *newDependance = malloc(sizeof(Dependance));
	newDependance->dependant = current;
	newDependance->dependsOn = tid;

	if (AppendFila2(&filaJoin, (void*)newDependance))
		return -1;

	// We need to decide on what is going to run since this is getting blocked.

	nextThread = schedule();

	switch (nextThread){
		case 0:
			next = (TCB_t*)GetAtIteratorFila2(&aptos0);
			if (DeleteAtIteratorFila2(&aptos0)){
				return -1;
			}
			break;

		case 1:
			next = (TCB_t*)GetAtIteratorFila2(&aptos1);
			if (DeleteAtIteratorFila2(&aptos1)){
				return -1;
			}
			break;

		case 2:
			next = (TCB_t*)GetAtIteratorFila2(&aptos2);
			if (DeleteAtIteratorFila2(&aptos2)){
				return -1;
			}
			break;

		case 3:
			next = (TCB_t*)GetAtIteratorFila2(&aptos3);
			if (DeleteAtIteratorFila2(&aptos3)){
				return -1;
			}
			break;

		default:
			return -1;
			break;
	}

	currentContext = &(current->context);
	nextContext = &(next->context);

	if (AppendFila2(&executando, next)){
		return -1;
	}

	swapcontext(currentContext,nextContext);

	return 0;
}

int csem_init(csem_t *sem, int count){
	FILA2 *queue = malloc(sizeof(FILA2));
	sem->count = count;
	if(CreateFila2(queue)){
		return -1;
	}
	sem->fila = queue;

	if(AppendFila2(&filaWait, (void*)sem)){
		return -1;
	}

	return 0;
}

int cwait(csem_t *sem){

	if (sem->count <= 0){
		// There are not enough free resources.
		// We must block it
		// i.e., remove it from running, insert it in this queue and select a new thread to run.
		TCB_t *current;
		TCB_t *next;
		ucontext_t *currentContext;
		ucontext_t *nextContext;
		int nextThread;

		if (FirstFila2(&executando)){
			return -1;
		}

		current = GetAtIteratorFila2(&executando);

		if (DeleteAtIteratorFila2(&executando)){
			return -1;
		}

		if (AppendFila2(sem->fila, current)){
			return -1;
		}

		nextThread = schedule();

		switch (nextThread){
			case 0:
				next = (TCB_t*)GetAtIteratorFila2(&aptos0);
				if (DeleteAtIteratorFila2(&aptos0)){
					return -1;
				}
				break;

			case 1:
				next = (TCB_t*)GetAtIteratorFila2(&aptos1);
				if (DeleteAtIteratorFila2(&aptos1)){
					return -1;
				}
				break;

			case 2:
				next = (TCB_t*)GetAtIteratorFila2(&aptos2);
				if (DeleteAtIteratorFila2(&aptos2)){
					return -1;
				}
				break;

			case 3:
				next = (TCB_t*)GetAtIteratorFila2(&aptos3);
				if (DeleteAtIteratorFila2(&aptos3)){
					return -1;
				}
				break;

			default:
				return -1;
				break;
		}

		currentContext = &(current->context);
		nextContext = &(next->context);

		if (AppendFila2(&executando, next)){
			return -1;
		}

		sem->count--;

		swapcontext(currentContext,nextContext);

		return 0;

	}

	sem->count--;

	return 0;
}
int csignal(csem_t *sem){

	if (sem->count < 0){
		// There are threads currently blocked, waiting for a signal.
		// We must "unblock it"
		// i.e., remove it from this queue and place it on ready

		TCB_t *element;
		int prio;

		if(FirstFila2(sem->fila)){
			return -1;
		}

		element = (TCB_t*)GetAtIteratorFila2(sem->fila);

		if(DeleteAtIteratorFila2(sem->fila)){
			return -1;
		}

		prio = element->ticket;


		switch(prio){
			case 0:
				if(AppendFila2(&aptos0, (void*)element)){
					return -1;
				}
				break;
			case 1:
				if(AppendFila2(&aptos1, (void*)element)){
					return -1;
				}
				break;
			case 2:
				if(AppendFila2(&aptos2, (void*)element)){
					return -1;
				}
				break;
			case 3:
				if(AppendFila2(&aptos3, (void*)element)){
					return -1;
				}
				break;
			default:
				return -1;
				break;
		}

	}

	sem->count++;

	return 0;
}

int cidentify (char *name, int size){
	static const char idString[] = "Bruno Loureiro - 260725\nEduardo Bassani - 261591";
	int nChars = size;
	int length = strlen(idString)+1;

	if (size > length){
		nChars = length;
	}

	memcpy(name, idString, nChars);

	return 0;
}