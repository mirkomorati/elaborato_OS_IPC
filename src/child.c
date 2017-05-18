#include "../headers/child.h"

int child(shm_t **shm_array, int pipe_fd, int queue_id, lock_t *sem_ids){
	cmd_t cmd;
	while(true){
		if(rcv_cmd(&cmd, pipe_fd) == -1)
			perror("read fallita\n");
		else{
			switch (cmd.role) {
				case MULTIPLY:
					#ifdef DEBUG
					printf("FIGLIO %i\tMULTIPLY\ti: %i, j: %i\n", getpid(), cmd.data.c.i, cmd.data.c.j);
					#endif
					multiply(cmd.data.c.i, cmd.data.c.j, shm_array, sem_ids);
				break;
				case SUM:
					#ifdef DEBUG
					printf("FIGLIO %i\tSUM\trow: %i\n", getpid(), cmd.data.row);
					#endif
					sum(cmd.data.row, shm_array, sem_ids);
				break;
				case END:
					return 0;
			}
			if(cmd.role == END) return 0;

			msg_t msg;
			msg.type = 1;
			msg.success = true;
			msg.cmd = cmd;
			msg.pid = getpid();

			if(send_msg(&msg, queue_id, sem_ids->queue_sem) == -1) {
				perror("send_msg child");
				return -1;
			}
		}
	}
	return 0;
}

int multiply(int i, int j, shm_t **shm_array, lock_t *sem_ids) {
	shm_t *A = shm_array[0];
	shm_t *B = shm_array[1];
	shm_t *C = shm_array[2];

	long *row = (long *) malloc(sizeof(long) * A->N);
	long *col = (long *) malloc(sizeof(long) * B->N);

	if (sem_lock(sem_ids->A_sem, i) == -1) {
		perror("ERROR multiply - sem_lock A");
		return -1;
	}
	/*#ifdef DEBUG
	printf("SEM A LOCK da figlio %i, riga %i\n", getpid(), i);
	#endif*/
	for (int l = 0; l < A->N; l++) {
		row[l] = A->shmaddr[i * A->N + l];
		/*#ifdef DEBUG
		printf("%i: %li\t", l, row[l]);
		#endif*/
	}
	sem_unlock(sem_ids->A_sem, i);
	/*#ifdef DEBUG
	printf("\nSEM A UNLOCK da figlio %i, riga %i\n", getpid(), i);
	#endif*/

	if (sem_lock(sem_ids->B_sem, j) == -1) {
		perror("ERROR multiply - sem_lock B");
		return -1;
	}
	/*#ifdef DEBUG
	printf("SEM B LOCK da figlio %i, riga %i\n", getpid(), j);
	#endif*/
	for (int l = 0; l < B->N; l++) {
		col[l] = B->shmaddr[j + l * B->N];
		/*#ifdef DEBUG
		printf("%i: %li\t", l, col[l]);
		#endif*/
	}
	sem_unlock(sem_ids->B_sem, j);
	/*#ifdef DEBUG
	printf("\nSEM B UNLOCK da figlio %i, riga %i\n", getpid(), j);
	#endif*/

	long res = 0;
	for (int l = 0; l < A->N; l++) {
		res += row[l] * col[l];
	}

	/*#ifdef DEBUG
	printf("MULTIPLY\ti: %i, j: %i = %li\n", i, j, res);
	#endif*/

	if (sem_lock(sem_ids->result_sem, 0) == -1) {
		perror("ERROR multiply - sem_lock C");
		return -1;
	}
	C->shmaddr[i + j * C->N] = res;
	sem_unlock(sem_ids->result_sem, 0);

	free(row);
	free(col);

	return 0;
}

int sum(int k, shm_t **shm_array, lock_t *sem_ids) {
	#ifdef DEBUG
	printf("SUM\t da implementare\n");
	#endif
	return 0;
}
