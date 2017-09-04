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

// lines do not execeed 2000 chars
#define LINE_SIZE 2500

static void yash(char *);

char **ENVP = NULL;
static void printenv() {
	for (char **env = ENVP; *env != NULL; env++)
		printf("%s\n", *env);
}

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

static void yash(char *line) {
	bool isBackground = strrchr(line, '&') ? true : false;
	printf("isBackground: %s\n", isBackground ? "yes" : "no");

	int cpid = fork();
	if (cpid < 0) {
		fprintf(stderr, "yash: failed to spawn new process: exiting...\n");
		exit(errno);
	} else if (cpid == 0) {
		setpgid(0, 0);	// start new pgrp for job. 
		// printf("child pgrp: %d\n", getpgrp());
		Job *job = Job_new(getpgrp(), line);
		Job_execute(job);
		exit(errno);
	} else {
		// parent process
		// printf("parent pgrp: %d\n", getpgrp());
		wait(NULL);
	}
}

