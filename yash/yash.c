/** yash.c
 * yash shell for OS class. (Lab 1)
 *
 * Author: Ronald Macmaster
 * eid: rpm953
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char **argv) {
	int cpid = fork();
	if(cpid < 0) {
		fprintf(stderr, "failed to fork. exiting...\n");
	} else if(cpid == 0) { 
		// child process
		char *args[3];
		args[0] = "ls";
		args[1] = "-l";
		args[2] = (char *) NULL;
		execvp(args[0], args);
	} else { 
		// parent process
		wait(NULL);
	}
}
