#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

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
 
void Fibo(int *arg) {
	int value = *arg;
	int n = 1;
	while(n<=value) {
	
		printf("FIBO termo %d : %d\n", n, runFibo(n));
		n++;
		
		if(n<=value) {

			cyield();
		}		
		
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


void TRI(int *arg) {
	int value = *arg;

	int n = 1;
	while(n<=value) {
	
		printf("TRI termo %d : %d\n", n, runTRI(n));
		n++;
		
		if(n<=value) {

			cyield();
		}		
		
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

void PA(int *arg) {
	int value = *arg;

	int n = 1;
	while(n<=value) {
	
		printf("PA termo %d : %d\n", n, runPA(n));
		n++;
		
		if(n<=value) {

			cyield();
		}		
		
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

void PG(int *arg) {
	int value = *arg;
	int n = 1;
	while(n<=value) {
	
		printf("PG termo %d : %d\n", n, runPG(n));
		n++;
		
		if(n<=value) {

			cyield();
		}		
	}
}


int main(int argc, char const *argv[])
{
	//printf("%d %d %d %d\n", Fibo(10), TRI(10), PA(10), PG(10));
	int argPA 	= 8;
	int argPG 	= 10;
	int argFibo = 12;
	int argTRI 	= 6;
	
	int idPA 	= ccreate((void*)&PA, (void*)&argPA, 0);
	int idPG 	= ccreate((void*)&PG, (void*)&argPG, 0);
	int idFibo	= ccreate((void*)&Fibo, (void*)&argFibo, 0);
	int idTRI 	= ccreate((void*)&TRI, (void*)&argTRI, 0);

	cjoin(idPA);
	cjoin(idPG);
	cjoin(idFibo);
	cjoin(idTRI);

   	//printf("Fim\n");
   	return 0;
}