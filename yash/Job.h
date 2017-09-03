/** Job.h
 * represents a yash job.
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#include "bool.h"

#ifndef JOB_H
#define JOB_H

typedef struct {
	int pgid; // process group id.
	char *line; // command that was run.
	bool isRunning; // Running or Stopped
} Job;

#endif
