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

static List *JobList;		// linked list for job tracking.

static void yash(char *);
static void resume(bool fg);
static void foreground(int pgid);
static void updateJobList();
static void printJobList();

char **ENVP = NULL;
#ifdef DEBUG
static void printenv() {
	for (char **env = ENVP; *env != NULL; env++)
		printf("%s\n", *env);
}
#endif

int main(int argc, char **argv, char **envp) {
	char line[LINE_SIZE];
	const char prompt[] = "# ";

	// globals
	ENVP = envp;
	JobList = List_new();
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);

	printf(prompt);
	while (fgets(line, sizeof(line), stdin)) {
		char *head = strtok(strdup(line), " \n\t");
		if (!head) {
			updateJobList();
		} else if (!strcmp("jobs", head)) {
			printJobList();
		} else if (!strcmp("fg", head)) {
			resume(true); 
		} else if (!strcmp("bg", head)) {
			resume(false);
		} else if (!strcmp("exit", head)) {
			break; // exit built in
		} else {
			yash(line);
		}
		printf(prompt);
	}

	// release resources
	List_free(JobList, Job_free);
	printf("\n");
	return errno;
}

static void yash(char *line) {
	updateJobList();

	// next job num
	Job *top = ((Job *) List_get(JobList, 0));
	int number = top ? top->number + 1 : 1; 

	// push job to stack
	Job *job = Job_new(number, line);
	List_insert(JobList, 0, job);
	Job_execute(job);

	if (!strrchr(line, '&')) {
		foreground(job->pgid);
	} else {	
		printf("[%d] + %s\t%s", job->number, Job_status(job), job->line);
	}
}

/**
 * resumes a process.
 * fg: {true: resume in fg, false: resume in bg.}
 */
static void resume(bool fg) {
	updateJobList();
	if (JobList->size > 0) {
		Job *job = ((Job *) List_get(JobList, 0));
		kill(-(job->pgid), SIGCONT);
		job->stopped = false;
		
		printf("[%d] + %s\t%s", job->number, Job_status(job), job->line);
		if(fg) { // resume in foreground.
			foreground(job->pgid);
		}
	} else {
		fprintf(stderr, "yash: %s: current: no such job\n", fg ? "fg" : "bg");
	}
}


/**
 * watches a process group in the foreground.
 */
static void foreground(int pgid) {
	int wstatus = 0;
	tcsetpgrp(STDIN_FILENO, pgid);
	while(waitpid(-pgid, &wstatus, WUNTRACED) >= 0) {
		if (WIFSTOPPED(wstatus)) {
			// stopped with Ctrl-Z
			// printf("caught ctrl-Z.\n");
			Job *job = (Job *) List_get(JobList, 0);
			job->stopped = true;
			printf("\n");
			break;
		} else if (WIFSIGNALED(wstatus)) {
			// terminated with Ctrl-C 
			// printf("caught ctrl-C.\n");
			List_remove(JobList, 0, Job_free);
			printf("\n");
			break;
		}
	}

	// regain terminal control.
	tcsetpgrp(STDIN_FILENO, getpgid(0));

	// check wait status.
	if (WIFEXITED(wstatus)) {
		List_remove(JobList, 0, Job_free);
	} 
}

/**
 * checks for completed tasks.
 * prints completed tasks if they're done.
 */
static void updateJobList() {
	for (int i = JobList->size - 1; i >= 0; --i) {
		Job *job = (Job *) (List_get(JobList, i));
		const char *status = Job_status(job);	
		char flag = i == 0 ? '+' : '-';
		if (!strcmp("Done", status)) {
			printf("[%d] %c %s\t%s", job->number, flag, status, job->line);
			List_remove(JobList, i, Job_free);
		}
	}
}

/**
 * prints the job list.
 * most recent task has a +
 */
static void printJobList() {
	for (int i = JobList->size - 1; i >= 0; --i) {
		Job *job = (Job *) (List_get(JobList, i));
		const char *status = Job_status(job);	
		char flag = i == 0 ? '+' : '-';
		printf("[%d] %c %s\t%s", job->number, flag, status, job->line);
		if (!strcmp("Done", status)) {
			List_remove(JobList, i, Job_free);
		}
	}
}
