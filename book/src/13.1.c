#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int x = 2;
	printf("location of code : %p\n", (void *) main);
	printf("location of heap : %p\n", (void *) malloc(1));
	printf("location of stack : %p\n", (void *) &x);
	return 0;
}
