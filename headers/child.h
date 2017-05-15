#ifndef CHILD_H
#define CHILD_H

#include "std_lib.h"
#include "shm_lib.h"

void child(shm_t **shm_array, int pipe_fd, int queue_id);

#endif