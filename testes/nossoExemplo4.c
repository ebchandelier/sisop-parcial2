#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/support.h"
#include "../include/cthread.h"

int main(int argc, char *argv[ ]) {
	char str1[300];
	char str2[25];
	int ret_id;

	ret_id = cidentify(str1, 300);
	printf("ret %d -- %s\n", ret_id, str1);

	ret_id = cidentify(str2, 25);
	printf("ret %d -- %s\n", ret_id, str2);

	return 0;
}