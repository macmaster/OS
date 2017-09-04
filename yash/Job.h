/** Job.h
 * represents a yash job.
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#ifndef JOB_H
#define JOB_H

typedef struct {
	int pgid; // process group id.
	char *line; // command that was run.
} Job; 

/**
 * creates a new job object.
 */
Job *Job_new(int pgid, const char *line);

/**
 * frees a job object.
 */
void Job_free(void *job);

/**
 * start executing a job.
 * kicks off all the jobs in process id.
 */
void Job_execute(Job *job);

#endif
