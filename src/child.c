#include "../headers/child.h"

void child(shm_t **shm_array, int pipe_fd, int queue_id){
#ifdef DEBUG
	printf("sono il figlio %i\n", getpid());
#endif
}