/** yash.c
 * yash shell for OS class. (Lab 1)
 *
 * Author: Ronald Macmaster
 * EID: rpm953
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "List.h"
#include "Job.h"
#include "bool.h"
#include "yash.h"

// lines do not execeed 2000 chars
#define LINE_SIZE 2500

static int split(char **, char *);

char **ENVP = NULL;
static void printenv();

int main(int argc, char **argv, char **envp) {
	char line[LINE_SIZE];
	const char prompt[] = "# ";
	ENVP = envp;

	printf(prompt);
	while (fgets(line, sizeof(line), stdin)) {
		yash(line);
		printf(prompt);
	}

	return errno;
}

void yash(char *line) {
	bool isPipe = strchr(line, '|') ? true : false;
	bool isBackground = strrchr(line, '&') ? true : false;

	char *args[LINE_SIZE / 2];
	int count = split(args, line);

	int cpid = fork();
	if (cpid < 0) {
		fprintf(stderr, "yash: failed to spawn new process! exiting...\n");
		exit(errno);
	} else if (cpid == 0) {
		// child process
		// redirs(tokens, count);
		exit(execvp(args[0], args));
	} else {
		// parent process
		wait(NULL);
	}
}

static int split(char **tokens, char *line) {
	const char *delim = " \t\n";
	char *token = strtok(line, delim);
	int count = 0;

	while (token != NULL) {
		// printf("%d: %s\n", count, token);
		tokens[count] = strdup(token);
		token = strtok(NULL, delim);
		count += 1;
	}

	tokens[count] = (char *)NULL;
	return count;
}

static void printenv() {
	for (char **env = ENVP; *env != NULL; env++)
		printf("%s\n", *env);
}
