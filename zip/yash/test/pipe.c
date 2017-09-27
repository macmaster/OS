#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main () {
	char *ls[3] = { "ls", "-l", NULL };
	char *tac[2] = { "tac", NULL };
	char *cat[2] = { "cat", NULL };
	char *grep[3] = { "grep", "pipe", NULL, };
	char **commands[3] = {
		ls, tac, NULL
	};


	int n = 2;
	int prev[2], curr[2];
	for(int i = 0; i < n; i++) {
		printf("%d) %s\n", getpid(), commands[i][0]);
		prev[0] = curr[0];
		prev[1] = curr[1];
		if(i < (n - 1)) {
			pipe(curr);
		}

		int cpid = fork();
		if(cpid > 0) {
			if(i > 0) {
				dup2(prev[0], STDIN_FILENO);
				close(prev[1]);
				close(prev[0]);
			} 

			if(i < (n - 1)) {
				dup2(curr[1], STDOUT_FILENO);
				close(curr[0]);
				close(curr[1]);
			}

			execvp(commands[i][0], commands[i]);
			fprintf(stderr, "bad command: %d \n", commands[i][0]);
			exit(-1);
		}
	}

}
