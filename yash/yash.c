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

static int FgGroup;		// current foreground process.
static List *JobList;		// linked list for job tracking.

static void yash(char *);
static void resume(bool fg);
static void foreground(int pgid);
static void signalHandler(int signum);
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
	FgGroup = -1;
	JobList = List_new();
	signal(SIGINT, signalHandler);
	signal(SIGTSTP, signalHandler);
	signal(SIGCHLD, signalHandler);
	signal(SIGPIPE, signalHandler);

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
	int cpid = fork();
	if (cpid < 0) {
		fprintf(stderr, "yash: failed to spawn new process: exiting...\n");
		List_free(JobList, Job_free);
		exit(errno);
	} else if (cpid == 0) {
		// child process
		setsid();	// start new pgrp for job. 
		Job *job = Job_new(getpgrp(), line);
		job->stopped = false;
		Job_execute(job);
		exit(errno);
	} else {
		// create a new job tracker
		Job *job = Job_new(cpid, line);
		if (JobList->size > 0) {
			job->number = ((Job *) List_get(JobList, 0))->number + 1;
		} else {	// first job
			job->number = 1;
		}

		// push job to stack
		List_insert(JobList, 0, job);
		job->stopped = false;

		while((waitpid(-cpid, NULL, WNOHANG) == -1) && (errno == ECHILD)){
		// 	printf("waiting for %d to be created...\n", cpid);
		}

		if (!strrchr(line, '&')) {
			foreground(cpid);
		} else {	
			printf("[%d] + %s\t%s", job->number, Job_status(job), job->line);
		}
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
	FgGroup = pgid;
	
	int wstatus = 0, task = 0;
	while((task = waitpid(-pgid, &wstatus, WUNTRACED)) >= 0){
		printf("finished waiting for %d in group %d.\n", task, -pgid);
	}

	printf("task returned errno: %d\n", errno);
	printf("ECHILD: %d\n", ECHILD);
	printf("EINTR: %d\n", EINTR);

	// check wait status.
	if (WIFEXITED(wstatus)) {
		List_remove(JobList, 0, Job_free);
	} else if (WIFSIGNALED(wstatus)) {
		// terminated with Ctrl-C 
		List_remove(JobList, 0, Job_free);
	} else if (WIFSTOPPED(wstatus)) {
		// stopped with Ctrl-Z
		Job *job = (Job *) List_get(JobList, 0);
		job->stopped = true;
	}

	FgGroup = -1;
}

/**
 * catches and forwards signals.
 */
static void signalHandler(int signum) {
	if ((signum == SIGINT) && (FgGroup > 0)) {
		printf("fg: %d termed with ctrl c. \n", FgGroup);
		kill(-FgGroup, SIGINT);	// Ctrl-C
	} else if ((signum == SIGTSTP) && (FgGroup > 0)) {
		printf("fg: %d stopped with ctrl z. \n", FgGroup);
		kill(-FgGroup, SIGSTOP);	// Ctrl-Z
	} else if (signum == SIGCHLD) {
		// child signal (orphans and zombies?)
	} else if (signum == SIGPIPE) { 
		fprintf(stderr, "SIGPIPE caught from process %d! \n", getpid());
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
