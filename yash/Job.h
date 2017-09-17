/** Job.h
 * represents a yash job.
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#ifndef JOB_H
#define JOB_H

#include "bool.h"

typedef struct {
	int pgid; // process group id.
	int number; // job number.
	bool stopped; // job is stopped.
	char *line; // command that was run.
} Job; 

/**
 * creates a new job object.
 */
Job *Job_new(int number, const char *line);

/**
 * frees a job object.
 */
void Job_free(void *job);

/**
 * start executing a job.
 * kicks off all the jobs in process id.
 */
void Job_execute(Job *job);

/**
 * returns the status of a job.
 * {Running, Stopped, Done}
 */
const char *Job_status(Job *job);

#endif
