#include "../headers/child.h"

int child(int child_id, shm_t **shm_array, int pipe_fd, int queue_id, lock_t *sem_ids){
	cmd_t cmd;
	while (true) {
		if (rcv_cmd(&cmd, pipe_fd, child_id, sem_ids->pipe_sem) == -1)
			perror("read fallita\n");
		else {
			switch (cmd.role) {
				case MULTIPLY:
					#ifdef DEBUG
					sys_print(STDOUT, "FIGLIO %i\tMULTIPLY\ti: %i, j: %i\n", child_id, cmd.data.c.i, cmd.data.c.j);
					#endif
					multiply(cmd.data.c.i, cmd.data.c.j, shm_array);
				break;
				case SUM:
					#ifdef DEBUG
					sys_print(STDOUT, "FIGLIO %i\tSUM\t\trow: %i\n", child_id, cmd.data.row);
					#endif
					sum(cmd.data.row, shm_array, sem_ids);
				break;
				case END:
					sys_print(STDOUT, "FIGLIO %i\tEND\n", getpid());
					return 0;
			}

			msg_t msg;
			msg.type = 1;
			msg.success = true;
			msg.cmd = cmd;
			msg.id = child_id;

			if(send_msg(&msg, queue_id, sem_ids->queue_sem) == -1) {
				perror("send_msg child");
				return -1;
			}
		}
	}
	return 0;
}

int multiply(int i, int j, shm_t **shm_array) {
	shm_t *A = shm_array[0];
	shm_t *B = shm_array[1];
	shm_t *C = shm_array[2];

	long *row = (long *) malloc(sizeof(long) * A->N);
	long *col = (long *) malloc(sizeof(long) * B->N);

	for (int l = 0; l < A->N; l++) {
		row[l] = A->shmaddr[i * A->N + l];
	}

	for (int l = 0; l < B->N; l++) {
		col[l] = B->shmaddr[j + l * B->N];
	}

	long res = 0;
	for (int l = 0; l < A->N; l++) {
		res += row[l] * col[l];
	}

	C->shmaddr[i * C->N + j] = res;

	free(row);
	free(col);

	return 0;
}

int sum(int k, shm_t **shm_array, lock_t *sem_ids) {
	shm_t *C = shm_array[2];
	shm_t *S = shm_array[3];

	long res = 0;

	for (int i = 0; i < C->N; i++) {
		res += C->shmaddr[k * C->N + i];
	}

	if (sem_lock(sem_ids->S_sem, 0) == -1) {
		perror("ERROR sum - sem_lock S");
		return -1;
	}
	S->shmaddr[0] += res;
	sem_unlock(sem_ids->S_sem, 0);

	return 0;
}
