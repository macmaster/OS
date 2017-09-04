/** Job.c
 * Represents a yash job.
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bool.h"
#include "Job.h"

/* typedef struct {
	int pgid; // process group id.
	char *line; // command that was run.
} Job; */

static char *strclone(const char *token);
static int split(char **tokens, const char *line);

/**
 * Creates a new job object.
 */
Job *Job_new(int pgid, const char *line) {
	Job *job = (Job *) malloc(sizeof(Job));
	if (job) {		
		job->pgid = pgid;
		job->line = strclone(line);
		return job;
	} else {
		return NULL;
	}
}

/**
 * Frees a Job object.
 */
void Job_free(void *job) {
	free(((Job *) job)->line);
	free(job);
}

/**
 * Starts executing a job.
 * Kicks off all the jobs in process id.
 */
void Job_execute(Job * job) {
	char *args[1250];
	int count = split(args, job->line);
	bool isPipe = strchr(job->line, '|') ? true : false;

	// printf("pgid: %d\n", job->pgid);
	// printf("command: %s", job->line);
	// printf("isPipe: %s\n", isPipe ? "yes" : "no");
	// printf("numTokens: %d\n", count);
	
	execvp(args[0], args);
	printf("yash: %s: command not found\n", args[0]);
}

/**
 * Splits a job line into tokens.
 * Returns number of tokens counted.
 */
static int split(char **tokens, const char *line) {
	const char *delim = " \t\n&";
	char *token = strtok(strclone(line), delim);
	int count = 0;

	while (token != NULL) {
		// printf("%d: %s\n", count, token);
		tokens[count] = strclone(token);
		token = strtok(NULL, delim);
		count += 1;
	}

	tokens[count] = (char *)NULL;
	return count;
}

/**
 * Duplicates a string object on the heap.
 */
static char *strclone(const char *token) {
	if (!token)
		return NULL;
	int len = strlen(token) + 1;
	char *word = (char *)malloc(len);
	return word != NULL ? strncpy(word, token, len + 1) : NULL;
}
