#ifndef FATHER_H
#define FATHER_H
#include "std_lib.h"
#include "shm_lib.h"
#include "ending_lib.h"
#include "sem_lib.h"
#include "child.h"

int init(shm_t **shm_array);

int make_child(shm_t **shm_array , int P, int *pipe_fd, int *queue_id);

int run(int P, int pipe, int queue);

#endif