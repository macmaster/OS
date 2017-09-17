/** Job.c
 * Represents a yash job.
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "bool.h"
#include "Job.h"

/* typedef struct {
	int pgid; // process group id.
	int number; // job number
	bool stopped; // job is stopped.
	char *line; // command that was run.
} Job; */

static int split(char **tokens, const char *line, const char *delim);

/**
 * Creates a new job object.
 */
Job *Job_new(int number, const char *line) {
	Job *job = (Job *) malloc(sizeof(Job));
	if (job) {
		job->pgid = -1; // invalid
		job->number = number;
		job->stopped = true;
		job->line = strdup(line);
		return job;
	} else {
		return NULL;
	}
}

/**
 * Frees a Job object.
 */
void Job_free(void *job) {
	kill(-((Job *) job)->pgid, SIGTERM);
	free(((Job *) job)->line);
	free(job);
}

/**
 * Starts executing a job.
 * Kicks off all the jobs in process id.
 */
void Job_execute(Job * job) {
	int prev[2], curr[2]; // pipes.
	char *command[1250], *args[1250];

	int numCommands = split(command, job->line, "|&\n");
	for (int i = 0; i < numCommands; ++i) {
		// create pipes.
		prev[0] = curr[0];
		prev[1] = curr[1];
		if(i < (numCommands - 1)) {
			pipe(curr);
		}
		
		int cpid = fork();
		if (cpid == 0) {
			// printf("[%d] (%d, %d, %d) %s\n", job->pgid, getpid(), getpgid(0), getsid(0), command[i]);
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			signal(SIGCHLD, SIG_DFL);
			signal(SIGPIPE, SIG_DFL);
			signal(SIGTTOU, SIG_DFL);


			// pipe input
			if(i > 0) { 
				dup2(prev[0], STDIN_FILENO);
				close(prev[1]); 
			}

			// pipe output
			if(i < (numCommands - 1)) {
				dup2(curr[1], STDOUT_FILENO);
				close(curr[0]);
			}

			split(args, command[i], " \n\t");
			execvp(args[0], args);
			fprintf(stderr, "yash: %s: command not found\n", args[0]);
			exit(errno);
		} else if (cpid > 0 && i == 0) {
			// first command is the group leader.
			setpgid(cpid, cpid);
			job->pgid = cpid;
			job->stopped = false;
		} else if (cpid > 0 && i > 0) {
			// parent closes pipe.
			setpgid(cpid, job->pgid);
			close(prev[0]);
			close(prev[1]);
		} else if (cpid < 0) {
			fprintf(stderr, "yash: failed to fork new process: exiting...\n");
			exit(errno);
		}
	}
}

/**
 * returns the status of a job.
 * {Running, Stopped, Done}
 */
const char *Job_status(Job * job) {
	int wstatus = 0;
	int pid = waitpid(-(job->pgid), &wstatus, WNOHANG | WUNTRACED | WCONTINUED);
	if ((pid == -1) || (pid > 0 && WIFEXITED(wstatus))) {
		return strdup("Done");
	} else {
		return job->stopped ? strdup("Stopped") : strdup("Running");
	}
}

/**
 * Splits a job line into tokens.
 * Returns number of tokens counted.
 */
static int split(char **tokens, const char *line, const char *delim) {
	char *token = strtok(strdup(line), delim);
	int count = 0;

	while (token != NULL) {
		tokens[count] = strdup(token);
		token = strtok(NULL, delim);
		count += 1;
	}

	tokens[count] = (char *)NULL;
	return count;
}

